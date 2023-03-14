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

object cube = {{50,50,0.0},10.0,0,"cube",0};
float t = INFINITY;
vecteur n;
cout << cos(25);

bool intersects = hitCube(r, cube, t,n);
if (intersects == true)
    cout << "Intersecte";

}