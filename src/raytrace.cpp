// Ceci est le code principale du raytracer.
// Il contient :
//  - la fonction d'initialisation "init"
//  - la fonction d'itération et de calcul du raytracing "draw"
//  - la fonction main d'éxécution du code

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include "formes.hpp"

using namespace std;

/*************Initialisation : création de la scène à partir du fichier texte *************/
 bool init(char* inputName, scene &myScene) 
 {
   int nbMat, nbObjets, nbLight;
   int i;

   ifstream sceneFile(inputName);
   if (!sceneFile)
     return  false;
   sceneFile >> myScene.sizex >> myScene.sizey; // 1ère ligne du .txt
   sceneFile >> nbMat >> nbObjets >> nbLight;   // 2ème ligne du .txt
   
   // On dimensionne les vecteurs de notre scène en fonction des valeurs du .txt
   myScene.matTab.resize(nbMat); 
   myScene.objTab.resize(nbObjets); 
   myScene.lgtTab.resize(nbLight); 

   // Maintenant que c'est dimensionné, on remplit ces vecteurs avec les valeurs 
   for (i=0; i < nbMat; i++)             // Informations sur les matériaux (chaque matTab[i] contient 4 valeurs)
     sceneFile >> myScene.matTab[i];
   for (i=0; i < nbObjets; i++){       // Informations sur les objets (chaque objTab[i] contient 8 valeurs)
     sceneFile >> myScene.objTab[i];
    }

   for (i=0; i < nbLight; i++)           // Informations sur les lumières (chaque lgtTab[i] contient 6 valeurs)
     sceneFile >> myScene.lgtTab[i];
   
   return true;
 } 


/*************** CALCUL DU RAY TRACING ET CREATION DE L'IMAGE ***************/
 bool draw(char* outputName, scene &myScene) 
 {
  
   // Création du fichier output en binaire
   ofstream imageFile(outputName,ios_base::binary);

   // Vérification au cas où
   if (!imageFile)
     return false; 
   
   // Binaire : On ajoute le header spécifique au format TGA
   imageFile.put(0).put(0);  // - 2 premiers octets à 0
   imageFile.put(2);         // - 3ème octet mis à 2 pour dire que l'on est en RGB non compressé
   imageFile.put(0).put(0);  // - 2 Octets de l'origine en X mis à 0
   imageFile.put(0).put(0);  // - 2 Octets de l'origine en Y mis à 0

   imageFile.put(0);
   imageFile.put(0).put(0); 
   imageFile.put(0).put(0); 

   // Ici, le 0xFF00 sert a prendre uniquement les valeurs octet par octet (c'est un masque)
   // On divise par 256 pour le 2ème octet car diviser par 2^8 décale justement de 8 bits le résultat en binaire
   // Bref, ca sert juste a stocker la taille de l'image
   imageFile.put((myScene.sizex & 0x00FF)).put((myScene.sizex & 0xFF00) / 256); // - 2 octets de la largeur de l'image 
   imageFile.put((myScene.sizey & 0x00FF)).put((myScene.sizey & 0xFF00) / 256); // - 2 octets de la largeur de l'image
   
   imageFile.put(24); // On a 24 bits par pixel dans l'image
   imageFile.put(0);  // On finit le header avec un 0
   

   // Balayage des rayons lumineux
   for (int y = 0; y < myScene.sizey; ++y) { //On parcourt tous les pixels de l'image
   for (int x = 0; x < myScene.sizex; ++x) {
     float red = 0, green = 0, blue = 0;    
     float coef = 1.0f;
     int level = 0; 
     
     // lancer de rayon 
     ray viewRay = { {float(x), float(y), -10000.0f}, { 0.0f, 0.0f, 1.0f}}; // Le rayon est "perprendiculaire" à l'écran et commence a -10 000
     
     // Boucle while qui s'arrête après 10 itérations ou si on a une reflection négative 
     do 
     { 
       // recherche de l'intersection la plus proche
       float t = 30000.0f;     // On prend comme distance "infinie" 30 000
       int currentObject= -1;
       string currentObjectType = "None";

       for (unsigned int i = 0; i < myScene.objTab.size(); ++i) // Pour chacun des objets
       { 
        if (myScene.objTab[i].type == "sphere"){
          if (hitSphere(viewRay, myScene.objTab[i], t)){        // Si l'objet intersecte le rayon
            currentObject = i;
            currentObjectType = "sphere";
            }                                 // On le prend comme l'objet 'actuel'
        }
        if (myScene.objTab[i].type == "cube"){
          if (hitCube(viewRay, myScene.objTab[i], t)){        // Si l'objet intersecte le rayon
            currentObject = i;
            currentObjectType = "cube";
            }                                 // On le prend comme l'objet 'actuel'
        }
       }

       if (currentObject == -1)
         break;

       // On calcule le point de rencontre entre le rayon et l'objet
       point newStart = viewRay.start.pos + t * viewRay.dir; 
       vecteur n = newStart - myScene.objTab[currentObject].pos;       
       
       // On calcule la normale à ce point par rapport à l'objet actuel
       if (currentObjectType == "sphere"){                                 // 1er cas, la sphère
          n = newStart - myScene.objTab[currentObject].pos;}       // Pour cela, on fait juste pt_rencontre - centre de l'objet 
       
       if (currentObjectType == "cube"){                                   // 2ème cas, le cube
          n = newStart - myScene.objTab[currentObject].pos;}     // Ici, la ,normale dépend de la face du cube et de la rotation

       // On vérifie juste que cette normale est non nulle
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
         if (n * dist <= 0.0f)                                    // On vérifie que l'on prend uniquement les rayons sortants de l'objet
           continue;
        
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
         }

         // Si un objet est touché par le rayon réflechi, ca veut dire que la surface actuelle est dans l'ombre ! Donc rien à faire
         // En revanche, si il n'y a aucun objet entre la surface actuelle et la lumière :
         if (!inShadow) {
           // On applique le modèle de Lambert, ou lambert est la "valeur d'éclairement"
           float lambert = (lightRay.dir * n) * coef;          // De base, coef vaut 1
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
     while ((coef > 0.0f) && (level < 10));   
    
     // On met à jour le pixel de l'image
     imageFile.put((unsigned char)min(blue*255.0f,255.0f)).put((unsigned char)min(green*255.0f, 255.0f)).put((unsigned char)min(red*255.0f, 255.0f));
   }
   }
   return true;
 }

/************ Fonction main ***********/
 int main(int argc, char* argv[]) {
   if  (argc < 3)
     return -1;
   scene myScene;
   if (!init(argv[1], myScene))    // On initialise (indirectement)
     return -1;
   if (!draw(argv[2], myScene))    // On dessine (toujours indirectement)
     return -1;
   return 0;
 }
