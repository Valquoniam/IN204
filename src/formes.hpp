#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include "raytrace.hpp"

using namespace std;
// La structure des objets : 
//	- Un centre de gravité pos
//	- Une taille (dont la définition diffère pour chaque objet que l'on va décrire)
//	- Un matériau
struct object {
	point pos;
	float size;
	int material;
	string type;
	int angle_rotation;
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


/******************* Calculs mathématiques pour différentes formes *****************/

// Pour la sphère. Cette fonction marche 'pseudo-récursivement' dans la fonction draw, car la fonction ne continuera que si hitSphere renvoie True
bool hitSphere(const ray &r, const object &s, float &t) 
 { 
    
   // Calcul du discriminant
   vecteur dist = s.pos - r.start.pos;               // Vecteur view_ray -> centre de la sphère
   float B = r.dir * dist;                           // Produit scalaire entre la dist et la direction du rayon
   float D = B*B - dist * dist + s.size * s.size;    // Discriminant de l'équation polynomiale
   
   // On test si il y a intersection 
   if (D < 0.0f) 
     return false; 
   
   // Si il y a bien intersection, on calcul les 2 racines
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

// Pour le cube. On définit les 8 sommets du cube et on testera si le point appartient a une des 8 faces du cube
// Il faut faire cela pour CHAQUE face, donc c'est très répétitif à coder
bool hitCube(const ray &r, const object &c, float &t) 
 {  
    point center = c.pos;        // Le centre du cube
    int size = c.size;           // La taille d'un côté du cube
    int angle = c.angle_rotation; // L'angle de rotation du cube
    
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

        // rotation autour de l'axe Y
        float xrot = x * co - z * si;
        float zrot = z * co + x * si;

        //On met à jour les coordonnées après la rotation
        vertices[i].x = xrot + center.x;
        vertices[i].y = y + center.y;
        vertices[i].z = zrot + center.z;
        }

    // Tests des intersections entre le rayon et le cube (pas encore fait et jy arrive )

    
    return false;
 }

struct face {
    point p1, p2, p3, p4;
};

double distancePointToFace(point p, face f) {
    double a = f.p1.y * (f.p2.z - f.p3.z) + f.p2.y * (f.p3.z - f.p1.z) + f.p3.y * (f.p1.z - f.p2.z);
    double b = f.p1.z * (f.p2.x - f.p3.x) + f.p2.z * (f.p3.x - f.p1.x) + f.p3.z * (f.p1.x - f.p2.x);
    double c = f.p1.x * (f.p2.y - f.p3.y) + f.p2.x * (f.p3.y - f.p1.y) + f.p3.x * (f.p1.y - f.p2.y);
    double d = -(f.p1.x * (f.p2.y * f.p3.z - f.p3.y * f.p2.z) + f.p2.x * (f.p3.y * f.p1.z - f.p1.y * f.p3.z) + f.p3.x * (f.p1.y * f.p2.z - f.p2.y * f.p1.z));
    double distance = fabs(a * p.x + b * p.y + c * p.z + d) / sqrt(a * a + b * b + c * c);
    return distance;
};

// On a un point dont on sait qu'il est sur le cube et on veut savoir sur quelle face du cube il est
vecteur normale_face_cube(const point &p, const object &c){
    point center = c.pos;        // Le centre du cube
    int size = c.size;           // La taille d'un côté du cube
    int angle = c.angle_rotation; // L'angle de rotation du cube
    
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

        // rotation autour de l'axe Y
        float xrot = x * co - z * si;
        float zrot = z * co + x * si;

        //On met à jour les coordonnées après la rotation
        vertices[i].x = xrot + center.x;
        vertices[i].y = y + center.y;
        vertices[i].z = zrot + center.z;
        }

    face cube[6] = {
      {vertices[0], vertices[2], vertices[4],vertices[6]}, // Face avant intialement
      {vertices[1], vertices[3], vertices[5],vertices[7]}, // Face arrière initialement
      {vertices[0], vertices[1], vertices[2],vertices[3]}, // Face gauche
      {vertices[4], vertices[5], vertices[6],vertices[7]}, // Face droite
      {vertices[2], vertices[3], vertices[6],vertices[7]}, // Face dessus
      {vertices[0], vertices[1], vertices[4],vertices[5]}, // Face dessous
    };
    
    int face_point;
    float meilleure_distance = INFINITY;
    
    for (int i = 0; i < 6; i++) {
      if (distancePointToFace(p,cube[i]) < meilleure_distance){
          meilleure_distance = distancePointToFace(p,cube[i]);
          face_point = i;}
    } 
    vecteur normale = (cube[face_point].p1 - cube[face_point].p2) & (cube[face_point].p3 - cube[face_point].p4);
    return normale;
  }