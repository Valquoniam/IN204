#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include "../hpp/objects.hpp"

int main(){
vecteur dir = {0.0, 0.1, 1.0};
point pos = {50,50,-30};

vecteur v1 = {1,2,3};
vecteur v2 = {1,5,7};

vecteur v3 = v1 & v2;
ray r = {pos, 0,0,0, dir};

object c = {{50,50,0.0},10.0,0,"cube",45,45};
float t = INFINITY;
vecteur n;

point center = c.pos;           // Le centre du cube
    int size = c.size;              // La taille d'un côté du cube
    float angle_x = c.angle_rot_x; // L'angle de rotation du cube
    float angle_y = c.angle_rot_y;
    angle_x = angle_x *  0.0174533;    // Conversion de l'angle en radians
    angle_y = angle_y *  0.0174533;

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

  // Rotation du cube 
  rotateCube(center, vertices, angle_y, angle_x);

  // Faces du cube 
  face cube[6] = {
      {vertices[0], vertices[2], vertices[4],vertices[6]}, // Face avant intialement
      {vertices[7], vertices[7], vertices[5],vertices[1]}, // Face arrière initialement
      {vertices[0], vertices[1], vertices[2],vertices[3]}, // Face gauche
      {vertices[7], vertices[7], vertices[4],vertices[5]}, // Face droite
      {vertices[2], vertices[3], vertices[6],vertices[7]}, // Face dessus
      {vertices[0], vertices[0], vertices[1],vertices[5]}, // Face dessous
    };

  // calcule les vecteurs normaux des 6 faces du cube
vecteur normales[6];
  for (int i=0; i<6;i++){
    vecteur diago1 = cube[i].p1 - cube[i].p4;
    //cout << diago1.x << ", " <<diago1.y << ", " <<diago1.z << endl;
    vecteur diago2 = cube[i].p2 - cube[i].p3;
    //cout << diago2.x << ", " <<diago2.y << ", " <<diago2.z << endl;
    vecteur k = diago1 & diago2;
    float norm = 1/sqrtf((k.x)*(k.x) + (k.y)*(k.y) + (k.z)*(k.z));
    normales[i] = norm * k;
    if(fabs(normales[i].x)<0.0001)
      normales[i].x =0;
    if(fabs(normales[i].y)<0.0001)
      normales[i].y =0;
    if(fabs(normales[i].z)<0.0001)
      normales[i].z =0;

    cout << normales[i].x << ", " << normales[i].y << ", " << normales[i].z << endl;
    cout << endl;
    }


bool intersects = hitCube(r, c, t,n);
if (intersects == true)
    cout << "Intersecte";

}