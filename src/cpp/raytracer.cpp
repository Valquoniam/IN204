// Ceci est le code principale du raytracer.
// Il contient : la fonction d'itération et de calcul du raytracing "draw"

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include "../hpp/tga_image.hpp"
using namespace std;

/*************** CALCUL DU RAY TRACING ET CREATION DE L'IMAGE ***************/
 bool draw(char* outputName, scene &myScene) 
 {
   
   // Création du fichier output en binaire
   ofstream imageFile(outputName,ios_base::binary);

   // Header TGA
   if (!header_tga(outputName, myScene, imageFile)){
      std::cout << "Problème d'image" << endl;
      return false;
   }

   // Balayage des rayons lumineux
   for (int y = 0; y < myScene.sizey; ++y) { //On parcourt tous les pixels de l'image
    for (int x = 0; x < myScene.sizex; ++x) {
     float red = 0, green = 0, blue = 0;    
     float coef = 1.0f;
     int level = 0; 
     
     // Lancer de rayon 
     ray viewRay = { {float(x), float(y), -10000.0f}, { 0.0f, 0.0f, 1.0f}}; // Le 1er rayon est "perprendiculaire" à l'écran et commence a -10 000
     // Ce premier rayon est "virtuel" et sert surtout a quel objet appartient le pixel qu'on parcourt, et la normale de l'objet en ce point
     
     // Boucle while qui s'arrête après x itérations ou si on a une reflection négative 
     do 
     { 
       // recherche de l'intersection la plus proche
       float t = 30000.0f;     // On prend comme distance "infinie" 30 000
       int currentObject= -1;
       string currentObjectType = "None";
       vecteur n;
       float tsphere = t;
       for (unsigned int i = 0; i < myScene.objTab.size(); ++i)  // Pour chacun des objets
       { 
        if (myScene.objTab[i].type == "sphere"){
          if (hitSphere(viewRay, myScene.objTab[i], tsphere)){         // Si l'objet intersecte le rayon
            currentObject = i;
            currentObjectType = "sphere";                        // On le prend comme l'objet 'actuel'
            }                            
        }
        if (myScene.objTab[i].type == "cube"){
          if (hitCube(viewRay, myScene.objTab[i], t, n)){        // Si l'objet intersecte le rayon
            currentObject = i;
            currentObjectType = "cube";                          // On le prend comme l'objet 'actuel'
            }                                 
        } 
       }
        
       if (tsphere <= t){
        currentObjectType = "sphere";
        t = tsphere;
       } else {
        currentObjectType = "cube";
       }

       if (currentObject == -1)
         break;
      
       // On calcule le point de rencontre entre le rayon et l'objet
       point newStart = viewRay.start.pos + t * viewRay.dir; 
       
       // cout << "(" << newStart.x << ", " << newStart.y << ", " << newStart.z << " )" << endl;
       // On calcule la normale à ce point par rapport à l'objet actuel
       if (currentObjectType == "sphere"){                                  // 1er cas, la sphère
          n = newStart - myScene.objTab[currentObject].pos;}                // Pour cela, on fait juste pt_rencontre - centre de l'objet 
       
       // Pour le cube, n est calculé directement via la fonction hitCube

       // On vérifie juste que la normale est non nulle
       if ( n * n == 0.0f) 
         break; 

       // On normalise la normale (aha)
       n =  1.0f / sqrtf(n*n) * n; 
       
       // On va calculer l'éclairement en fonction également du matériau
       material currentMat = myScene.matTab[myScene.objTab[currentObject].material]; 
       
       for (unsigned int j = 0; j < myScene.lgtTab.size(); ++j) { // Pour chaque lumière
         light current = myScene.lgtTab[j];
         vecteur dist = current.pos - newStart;                   // On prend le vecteur entre la source et le point de rencontre 
         float t = sqrtf(dist * dist); 
         if ( t <= 0.0f )                        // On vérifie que le vecteur dist est non nul (et aussi inf pour les erreurs d'arrondi)
           continue;
         if (n * dist <= 0.0f){                  // On vérifie que l'on prend uniquement les rayons sortants de l'objet
           continue;}

         // On crée le rayon de lumière de reflection, qui part du point de rencontre et va vers la source lumineuse
         ray lightRay;
         lightRay.start.pos = newStart;
         lightRay.dir = (1/t) * dist;

         //Ce vecteur est crée pour la fonction hitCube, qui modifie n en faisant les tests de collision. 
         // Evidemment, on veut faire les calculs avec le n initial
         vecteur n_temp = n;

         // calcul des objets dans l'ombre de cette reflexion 
         bool inShadow = false; 
         for (unsigned int i = 0; i < myScene.objTab.size(); ++i) {   // Pour tous les objets
           
           if ((myScene.objTab[i].type == "sphere") && (hitSphere(lightRay, myScene.objTab[i], t))) {  //Si un objet est touché par le rayon réflechi
             inShadow = true; // On note inShadow comme étant True (cf. suite)
             break;
           }
           
           if ((myScene.objTab[i].type == "cube") && (hitCube(lightRay, myScene.objTab[i], t, n))) {  //Si un objet est touché par le rayon réflechi
             inShadow = true;
             break;
           }
         }

         // Si un objet est touché par le rayon réflechi, ca veut dire que la surface actuelle est dans l'ombre ! Donc rien à faire
         // En revanche, si il n'y a aucun objet entre la surface actuelle et la lumière :
         if (!inShadow) {
           // On applique le modèle de Lambert, ou lambert est la "valeur d'éclairement"
           float lambert = (lightRay.dir * n_temp) * coef;          // De base, coef vaut 1
           red += lambert * current.red * currentMat.red;      // On met à jour les valeurs des pixels
           green += lambert * current.green * currentMat.green;
           blue += lambert * current.blue * currentMat.blue;
         }
       }
         
       // On met à jour les rayons pour calculer la reflection suivante
       coef *= currentMat.reflection;            // On met à jour le coefficient de reflection
       float reflet = 2.0f * (viewRay.dir * n);  // reflet vaut 2.cos(viewray,n)
       
       viewRay.start.pos = newStart;             // On met à jour la 'position de la caméra' pour calculer les reflections suivantes
       viewRay.dir = viewRay.dir - reflet * n;   // On met à jour la direction comme étant l'ancienne direction - reflet*la normale

       level++;  // On passe au niveau d'itération suivant
     } 
     while ((coef > 0.0f) && (level < 20));   

    
    // modification de l'exposition : On définit le terme exposure comme on l'entend entre 0 et -1
      float exposure =  - 0.5f;
      blue = 1.0f - expf(blue * exposure);
      red = 1.0f - expf(red * exposure);
      green = 1.0f - expf(green * exposure);

      //Transformation gamma pour augmenter la qualité
      float invgamma = 0.45454545; // invgamma est égale à la valeur retenue par le standard sRGB
      blue = powf(blue, invgamma );
      red = powf(red, invgamma );
      green = powf(green, invgamma );

      // On met à jour le pixel de l'image
      imageFile.put((unsigned char)min(blue*255.0f,255.0f)).put((unsigned char)min(green*255.0f, 255.0f)).put((unsigned char)min(red*255.0f, 255.0f));
   }
   }
   return true;
 }


