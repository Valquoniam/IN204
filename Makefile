# Le compilateur
CC = g++
 
# Les flags
CFLAGS  = -g -Wall
RM = rm
MAKEFLAGS = -Wno-file-ignored -s -j

# La cible du code 
TARGET = main

.SILENT:
    
all: $(TARGET)

# Démarche :
#	- On compile raytrace.cpp
#	- On éxécute raytrace.exe avec comme arguments la scène (en 1ère) et l'image output (en 2ème)
#	- On convertit l'image .tga en .png (Il faut avoir installé la biblio graphicsmagick)

$(TARGET): src/hpp/objects.hpp src/hpp/structs.hpp src/hpp/tga_image.hpp
	echo "Construction des images..."

	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp     
	
	./$(TARGET) scenes/scene1.txt images/image1.tga
	./$(TARGET) scenes/scene2.txt images/image2.tga
	./$(TARGET) scenes/scene3.txt images/image3.tga
	./$(TARGET) scenes/scene4.txt images/image4.tga


	echo "Rendu des images..."
	convert images/image1.tga images/image1.png
	convert images/image2.tga images/image2.png
	convert images/image3.tga images/image3.png
	convert images/image4.tga images/image4.png

	$(RM) images/*.tga

	echo "Construction de la mosaïque..."
	convert images/image1.png -resize 1000x1000^ -gravity center -background black -extent 1000x1000 images/image1_resized.png
	convert images/image2.png -resize 1000x1000^ -gravity center -background black -extent 1000x1000 images/image2_resized.png
	convert images/image3.png -resize 1000x1000^ -gravity center -background black -extent 1000x1000 images/image3_resized.png
	convert images/image4.png -resize 1000x1000^ -gravity center -background black -extent 1000x1000 images/image4_resized.png
	montage images/image1_resized.png images/image2_resized.png images/image3_resized.png images/image4_resized.png -tile 2x2 -geometry +0+0 images/mosaic.png
	$(RM) images/*resized.png

	echo "Mosaïque finie!"
	eog images/mosaic.png
clean:
	$(RM)  $(TARGET) 
	$(RM)  images/*.png


image1:	
	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp
	./$(TARGET) scenes/scene1.txt images/image1.tga
	convert images/image1.tga images/image1.png
	$(RM) images/image1.tga
	eog images/image1.png

image2:	
	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp
	./$(TARGET) scenes/scene2.txt images/image2.tga
	convert images/image2.tga images/image2.png
	$(RM) images/image2.tga
	eog images/image2.png

image3:	
	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp
	./$(TARGET) scenes/scene3.txt images/image3.tga
	convert images/image3.tga images/image3.png
	$(RM) images/image3.tga
	eog images/image3.png

image4:	
	$(CC) $(CFLAGS) -o $(TARGET) src/cpp/$(TARGET).cpp
	./$(TARGET) scenes/scene4.txt images/image4.tga
	convert images/image4.tga images/image4.png
	$(RM) images/image4.tga
	eog images/image4.png