#include "raytrace.h"
#include <iostream>
using namespace std;

/* 1ère structure : la sphère*/
struct sphere {
	point pos;
	float size;
	int material;
};

istream & operator >> ( istream &inputFile, sphere& sph ) {
	return inputFile >> sph.pos >> sph.size >> sph.material;
}
