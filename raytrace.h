using namespace std;
#include <iostream>
#include <vector>


/*************Définition des différentes structures que l'on va utiliser *****************/

// Un point dans l'espace 3D dans les coordonées cartésiennes
struct point {
	float x, y, z;
};

// Retourne les coordonnées d'un point
istream & operator >> ( istream &inputFile,  point& p ) {
	return inputFile >> p.x >> p.y >> p.z ; 
}

// Un vecteur, également composé de 3 points
struct vecteur {
	float x, y, z;
};

// Retourne les coordonées d'un vecteur
istream & operator >> ( istream &inputFile,  vecteur& v ) {
	return inputFile >> v.x >> v.y >> v.z ; 
}

// Déplace un point en utilisant un vecteur
point operator + (const point&p, const vecteur &v){
	point p2={p.x + v.x, p.y + v.y, p.z + v.z };
	return p2;
}

// Pareil pour la soustraction (gain de temps)
point operator - (const point&p, const vecteur &v){
	point p2={p.x - v.x, p.y - v.y, p.z - v.z };
	return p2;
}

// Définition d'un vecteur comme différence de 2 points
vecteur operator - (const point&p1, const point &p2){
	vecteur v={p1.x - p2.x, p1.y - p2.y, p1.z - p2.z };
	return v;
}

// Multiplication d'un vecteur par un entier
vecteur operator * (float c, const vecteur &v)
{
	vecteur v2={v.x *c, v.y * c, v.z * c };
	return v2;
}

// Addition de 2 vecteurs
vecteur operator + (const vecteur&v1, const vecteur &v2){
	vecteur v={v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	return v;
}

// Soustraction de 2 vecteurs
vecteur operator - (const vecteur&v1, const vecteur &v2){
	vecteur v={v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	return v;
}

// Produit scalaire
float operator * (const vecteur&v1, const vecteur &v2 ) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Définition d'un matériau par sa couleur (en pourcentage RGB et son indice de réflection)
struct material {
	float red, green, blue, reflection;
};

// Retourne les pourcentages de couleur et l'indice de reflection d'un matériau
istream & operator >> ( istream &inputFile, material& mat ) {
	return inputFile >> mat.red >> mat.green >> mat.blue >> mat.reflection; 
}

// On définit une source lumineuse par sa position et sa couleur
struct light {
	point pos;
	float red, green, blue;
};

// Retourne les valeurs d'une source lumineuse
istream & operator >> ( istream &inputFile, light& lig ) {
	return inputFile >> lig.pos >> lig.red >> lig.green >> lig.blue;
}

// Un rayon lumineux a une source et va dans une direction
struct ray {
	light start;
	vecteur dir;
};

// La structure des objets : 
//	- Un centre de gravité pos
//	- Une taille (dont la définition diffère pour chaque objet que l'on va décrire)
//	- Un matériau
struct object {
	point pos;
	float size;
	int material;
	string type;
};

istream & operator >> ( istream &inputFile, object& obj ) {
	return inputFile >> obj.pos >> obj.size >> obj.material >> obj.type;
}

// Notre scène est constitué d'un certain nombre de :
//	- Matériaux
// 	- Sphères
// 	- Sources lumineuses
struct scene {
	vector<material> matTab;
	vector<object>   objTab;
	vector<light>    lgtTab;
	int sizex, sizey;         // La taille de la scène
};
