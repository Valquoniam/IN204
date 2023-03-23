#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include "structs.hpp"

using namespace std;

/********************************* STRUCTURE GLOBALE D'OBJET *********************************/
// La structure des objets : 
//	- Un centre de gravité pos
//	- Une taille (dont la définition diffère pour chaque objet que l'on va décrire)
//	- Un matériau
struct object {
	point pos;
	float size;
	int material;
	string type;
	float angle_rotation;
};

istream & operator >> ( istream &inputFile, object& obj ) {
	return inputFile >> obj.pos >> obj.size >> obj.material >> obj.type >> obj.angle_rotation;
}

// Notre scène est constitué d'un certain nombre de :
//	- Matériaux
// 	- Objets (sphères, cubes, etc...)
// 	- Sources lumineuses
struct scene {
	vector<material> matTab;
	vector<object>   objTab;
	vector<light>    lgtTab;
	int sizex, sizey;         // La taille de la scène
};
/*******************************************************************************************/

/**********************************  OBJET 1 : SPHERE **************************************/

bool hitSphere(const ray &r, const object &s, float &t) 
 { 
    
   // Calcul du discriminant
   vecteur dist = s.pos - r.start.pos;               // Vecteur view_ray -> centre de la sphère
   float B = r.dir * dist;                           // Produit scalaire entre la dist et la direction du rayon
   float D = B*B - dist * dist + s.size * s.size;    // Discriminant de l'équation polynomiale
   
   // On test si il y a intersection 
   if (D < 0.0f) 
     return false; 
   
   // Si il y a bien intersection, on calcule les 2 racines
   float t0 = B - sqrtf(D); 
   float t1 = B + sqrtf(D);
   
   bool retvalue = false; 

   // Si la première racine est positive ET plus petite que t (cf. la suite), on la met comme nouvelle intersection la plus proche
   if ((t0 > 0.1f) && (t0 < t)) 
   {
     t = t0;
     retvalue = true; 
   } 

   // Pareil pour la 2eme racine
   if ((t1 > 0.1f) && (t1 < t)) 
   {
     t = t1; 
     retvalue = true; 
   }

   //On retourne True uniquement si un pt d'intersection a été trouvé
   return retvalue; 
 }

/*******************************************************************************************/

/***********************************  OBJET 2 : CUBE ***************************************/

//Une face du cube correspond à 4 sommets du cube
struct face {
    point p1, p2, p3, p4;
};


bool hitCube(const ray &r, const object &c, float &t, vecteur &n) 
 {  
    point center = c.pos;           // Le centre du cube
    int size = c.size;              // La taille d'un côté du cube
    float angle = c.angle_rotation; // L'angle de rotation du cube
    angle = angle *  -0.0174533;    // Conversion de l'angle en radians

    // les sommets du cube
    //  - Face gauche (1) : 4 premiers sommets
    //  - Face droite (2) : 4 derniers sommets
    //  - Face avant  (3) : Sommets d'indice pairs
    //  - Face arrière(4) : Sommets d'indice impairs
    //  - Face dessus (5) : Sommets 2,3,6,7
    //  - Face dessous(6) : Sommets 0,1,4,5
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
        
        float zrot = y * si + z * co;
        float yrot = y * co - z * si; 

        //On met à jour les coordonnées après la rotation
        vertices[i].x = x + center.x;
        vertices[i].y = yrot + center.y;
        vertices[i].z = zrot + center.z;
        }

  // Faces du cube 
  face cube[6] = {
      {vertices[0], vertices[2], vertices[4],vertices[6]}, // Face avant intialement
      {vertices[7], vertices[7], vertices[5],vertices[1]}, // Face arrière initialement
      {vertices[0], vertices[1], vertices[2],vertices[3]}, // Face gauche
      {vertices[7], vertices[7], vertices[4],vertices[5]}, // Face droite
      {vertices[2], vertices[3], vertices[6],vertices[7]}, // Face dessus
      {vertices[0], vertices[0], vertices[1],vertices[5]}, // Face dessous
    };

  //Vecteurs normaux du cube
  vecteur normales[6];
  for (int i=0; i<6;i++){
    vecteur diago1 = cube[i].p1 - cube[i].p4;
    //cout << diago1.x << ", " <<diago1.y << ", " <<diago1.z << endl;
    vecteur diago2 = cube[i].p2 - cube[i].p3;
    //cout << diago2.x << ", " <<diago2.y << ", " <<diago2.z << endl;
    vecteur k = diago1 & diago2;
    float norm = 1/sqrtf((k.x)*(k.x) + (k.y)*(k.y) + (k.z)*(k.z));
    normales[i] = norm * k;
    //cout << normales[i].x << ", " << normales[i].y << ", " << normales[i].z << endl;
    // cout << endl;
  }

    // Tests des intersections entre le rayon et le cube
    
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

    // Calculate the intersection of the ray with each of the six faces of the cube
    
    float k; 
    float liste_k[6] = {300000, 300000, 300000, 300000, 300000, 300000}; // Valeurs quasi à l'infini
    vecteur intersection;
    bool intersects = false;   // True si le rayon intersecte le cube

    if (r.dir.x != 0.0f) { 
        //Face gauche
        k = (min_x - r.start.pos.x) / r.start.pos.x; 
        intersection.x = min_x;
        intersection.y = r.start.pos.y + k * r.dir.y;
        intersection.z = r.start.pos.z + k * r.dir.z;
        if (intersection.y >= min_y && intersection.y <= max_y &&
            intersection.z >= min_z && intersection.z <= max_z && k>0.0f) {
            liste_k[2] = k;
            intersects = true;
        }
        //face droite
        k = (max_x - r.start.pos.x) / r.start.pos.x;
        intersection.x = max_x;
        intersection.y = r.start.pos.y + k * r.dir.y;
        intersection.z = r.start.pos.z + k * r.dir.z;
        if (intersection.y >= min_y && intersection.y <= max_y &&
            intersection.z >= min_z && intersection.z <= max_z && k>0.0f) {
            liste_k[3] = k;
            intersects = true;
        }
    }
    
    if (r.dir.y != 0.0f) {
        //face bas
        k = (min_y - r.start.pos.y) / r.dir.y;
        intersection.x = r.start.pos.x + k * r.dir.x;
        intersection.y = min_y;
        intersection.z = r.start.pos.z + k * r.dir.z;
        if (intersection.x >= min_x && intersection.x <= max_x &&
            intersection.z >= min_z && intersection.z <= max_z && k>0.0f) {
            liste_k[4] = k;
            intersects = true;
        }
        //face haut
        k = (max_y - r.start.pos.y) / r.dir.y;
        intersection.x = r.start.pos.x + t * r.dir.x;
        intersection.y = max_y;
        intersection.z = r.start.pos.z + t * r.dir.z;
        if (intersection.x >= min_x && intersection.x <= max_x &&
            intersection.z >= min_z && intersection.z <= max_z && k>0.0f) {
            liste_k[5] = k;
            intersects = true;
        }
    }
    if (r.dir.z != 0.0f) {
        //face avant
        k = (min_z - r.start.pos.z) / r.dir.z;
        intersection.x = r.start.pos.x + k * r.dir.x;
        intersection.y = r.start.pos.y + k * r.dir.y;
        intersection.z = min_z;
        if (intersection.x >= min_x && intersection.x <= max_x &&
           intersection.y >= min_y && intersection.y <= max_y && k > 0.0f) {
           liste_k[0] = k;
           intersects = true;
        }
        
        //face arrière
        k = (max_z - r.start.pos.z) / r.dir.z;
        intersection.x = r.start.pos.x + k * r.dir.x;
        intersection.y = r.start.pos.y + k * r.dir.y;
        intersection.z = max_z;
        if (intersection.x >= min_x && intersection.x <= max_x &&
          intersection.y >= min_y && intersection.y <= max_y && k>0.0f) {
          liste_k[1] = k;
          intersects = true;
        }
     }

    if (intersects){
      if (min_element(begin(liste_k),end(liste_k))[0] < t)
        t = min_element(begin(liste_k),end(liste_k))[0];  // Valeur minimale : intersection la plus proche
      auto itr = find(begin(liste_k), end(liste_k), t);
      int num_face = distance(begin(liste_k), itr);
      n = normales[num_face];}
      
      //cout << "face de colision " << num_face << endl;}

return intersects;

}