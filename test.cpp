#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include "src/formes.hpp"

int main(){
vecteur dir = {0.0, 0.1, 1.0};
point pos = {50,50,-30};

ray r = {pos, 0,0,0, dir};
object cube = {{50,50,0.0},10.0,0,"cube",0};
float t =5.0;
bool intersects = hitCube(r, cube, t);
if (intersects == true)
    cout << "Intersecte";

}