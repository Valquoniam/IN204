# IN204

## IT Project w/ Axel Dumont - RayTracer

Pour compiler le projet, il suffit de faire 'make all' et les images résultantes s'affichent en .png.

La façon dont marche le make all est décrite dans le fichier Makefile

Elles sont également sauvegardées dans le dossier images.
​
Ces 2 packages sont nécessaires à la compilation :
​
```bash
sudo apt install graphicsmagick-imagemagick-compat
sudo apt install eog
```
Pour ne pas avoir de warning en utilisant eog, il faut installer ce package:

```bash
sudo apt install adwaita-icon-theme-full
```

### Description du système de scène 
​
#### Exemple de scène1 :
​
* 640 480                          // taille du viewport 
* 3 3 2                            // nbre de materiel, d'objets et de lumieres 
* 1.0 1.0 0.0 0.5                  // premier materiel: rouge vert bleu et coef de reflexion 
* 0.0 1.0 1.0 0.5                  // deuxieme materiel 
* 1.0 0.0 1.0 0.5                  // troisieme materiel 
* 233.0 290.0 0.0 100 0 sphere     // objet 1: posx, posy, posz, rayon, materiel_id nom_objet
* 407.0 290.0 0.0 100 1 sphere     // objet 2 
* 320.0 140.0 0.0 100 2 sphere     // objet 3 
* 0.0 240.0 -100.0 1.0 1.0 1.0     // light 1 : posx, posy, posz, intensité rouge, vert et bleu 
* 640.0 240.0 -10000.0 0.6 0.7 1.0 // light 2