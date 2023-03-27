#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdio.h>
#include "../hpp/objects.hpp"

using namespace std;

int main(){
vecteur dir = {0.0, 0.0, 1.0};
point pos = {53,50,-30};

ray r = {pos, 0,0,0, dir};

object c = {{50,50,0.0},10.0,0,"cube", -20, 0};
float t = INFINITY;
vecteur n;

bool intersects = hitCube(r, c, t,n);
if (intersects == true)
    cout << "Intersecte";

}