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
      cout << "Problème d'image" << endl;
      return false;
   }

   // Balayage des rayons lumineux
   for (int y = 0; y < myScene.sizey; ++y) { //On parcourt tous les pixels de l'image
   for (int x = 0; x < myScene.sizex; ++x) {
     float red = 0, green = 0, blue = 0;    
     float coef = 1.0f;
     int level = 0; 
     
     // lancer de rayon 
     ray viewRay = { {float(x), float(y), -10000.0f}, { 0.0f, 0.0f, 1.0f}}; // Le 1er rayon est "perprendiculaire" à l'écran et commence a -10 000
     // ce premier rayon est "virtuel" et sert surtout a quel objet appartient le pixel qu'on parcourt, et la normale de l'objet en ce point
     int arete = 0; 
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
      /**************************************************************/

       if (currentObjectType == "cube"){
        point center = myScene.objTab[currentObject].pos;        // Le centre du cube
        int size = myScene.objTab[currentObject].size;           // La taille d'un côté du cube
        float angle = myScene.objTab[currentObject].angle_rotation; // L'angle de rotation du cube
        angle = angle *  0.0174533;

        point vertices[8] = {
      {center.x - size / 2, center.y - size / 2, center.z - size / 2}, // Sommet en bas à gauche devant
      {center.x - size / 2, center.y - size / 2, center.z + size / 2}, // Sommet en bas à gauche derriere
      {center.x - size / 2, center.y + size / 2, center.z - size / 2}, // Sommet en haut à gauche devant
      {center.x - size / 2, center.y + size / 2, center.z + size / 2}, // Sommet en haut à gauche derrière
      {center.x + size / 2, center.y - size / 2, center.z - size / 2}, // Sommet en bas à droite devant
      {center.x + size / 2, center.y - size / 2, center.z + size / 2}, // Sommet en bas à droite derrière
      {center.x + size / 2, center.y + size / 2, center.z - size / 2}, // Sommet en haut à droite devant
      {center.x + size / 2, center.y + size / 2, center.z + size / 2}  // Sommet en haut à droite derrière
            };
        
        // effectuer la rotation du cube
        float si = sin(angle);
        float co = cos(angle);

        for (int i = 0; i < 8; i++) {
            float x = vertices[i].x - center.x;
            float y = vertices[i].y - center.y;
            float z = vertices[i].z - center.z;

        // rotation 
        
        float zrot = y * co + z * si;
        float yrot = y * co - z * si; 

        //On met à jour les coordonnées après la rotation
        vertices[i].x = x + center.x;
        vertices[i].y = yrot + center.y;
        vertices[i].z = zrot + center.z;
        }

        // Calculate the minimum and maximum x, y, and z values of the cube
        float min_x = vertices[0].x, max_x = vertices[0].x;
        float min_y = vertices[0].y, max_y = vertices[0].y;
        float min_z = vertices[0].z, max_z = vertices[0].z;
    
        for (int i = 1; i < 8; ++i) {
          min_x = min(min_x, vertices[i].x);
          max_x = max(max_x, vertices[i].x);
          min_y = min(min_y, vertices[i].y);
          max_y = max(max_y, vertices[i].y);
          min_z = min(min_z, vertices[i].z);
          max_z = max(max_z, vertices[i].z);
          }
        int y_arete;
        for (int i = 1; i < 8; ++i) {
          if (vertices[i].z == min_z){
            y_arete = vertices[i].y;
          }
        }
        point newStart = viewRay.start.pos + t * viewRay.dir;
        if (fabs(newStart.z -min_z) < 0.08 && y == y_arete && level == 0 ){
          arete = 1;
        }
       }
       // On calcule le point de rencontre entre le rayon et l'objet
       point newStart = viewRay.start.pos + t * viewRay.dir; 
       
       // cout << "(" << newStart.x << ", " << newStart.y << ", " << newStart.z << " )" << endl;
       // On calcule la normale à ce point par rapport à l'objet actuel
       if (currentObjectType == "sphere"){                                 // 1er cas, la sphère
          n = newStart - myScene.objTab[currentObject].pos;}       // Pour cela, on fait juste pt_rencontre - centre de l'objet 
       
       // On vérifie juste que cette normale est non nulle
       if ( n * n == 0.0f) 
         break; 

       // On normalise la normale (aha)
       n =  1.0f / sqrtf(n*n) * n; 
      vecteur n_temp = n;
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
         
         // calcul des objets dans l'ombre de cette reflection 
         bool inShadow = false; 
         for (unsigned int i = 0; i < myScene.objTab.size(); ++i) {   // Pour tous les objets
           
           if ((myScene.objTab[i].type == "sphere") && (hitSphere(lightRay, myScene.objTab[i], t))) {  //Si un objet est touché par le rayon réflechi
             inShadow = true; // On note inShadow comme étant True (cf. suite)
             break;
           }
           
           if ((myScene.objTab[i].type == "cube") && (hitCube(lightRay, myScene.objTab[i], t, n))) {  //Si un objet est touché par le rayon réflechi
             inShadow = true; // On note inShadow comme étant True (cf. suite)
             //cout << "shadow";
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
     while ((coef > 0.0f) && (level < 50));   

     // On met à jour le pixel de l'image
     // Si le pixel appartient à une arête du cube
     if (arete == 1){
      imageFile.put((unsigned char)255).put((unsigned char)255).put((unsigned char)255);}
     
     else{
      imageFile.put((unsigned char)min(blue*255.0f,255.0f)).put((unsigned char)min(green*255.0f, 255.0f)).put((unsigned char)min(red*255.0f, 255.0f));}
   }
   }
   return true;
 }


