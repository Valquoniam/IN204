#include "formes.h"
#include "raytrace.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
using namespace std;


/*************** POUR LA SPHERE *****************/
bool hitSphere(const ray &r, const sphere &s, float &t) 
 { 
   // intersection rayon/sphere 
   vecteur dist = s.pos - r.start.pos; 
   float B = r.dir * dist;
   float D = B*B - dist * dist + s.size * s.size; 
   if (D < 0.0f) 
     return false; 
   float t0 = B - sqrtf(D); 
   float t1 = B + sqrtf(D);
   bool retvalue = false;  
   if ((t0 > 0.1f) && (t0 < t)) 
   {
     t = t0;
     retvalue = true; 
   } 
   if ((t1 > 0.1f) && (t1 < t)) 
   {
     t = t1; 
     retvalue = true; 
   }
   return retvalue; 
 }