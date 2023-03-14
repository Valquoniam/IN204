# the compiler: gcc for C program, define as g++ for C++
CC = g++
 
# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -g -Wall
RM = rm

# The build target 
TARGET = main

all: $(TARGET)

# Démarche :
#	- On compile raytrace.cpp
#	- On éxécute raytrace.exe avec comme arguments la scène (en 1ère) et l'image output (en 2ème)
#	- On convertit l'image .tga en .png (Il faut avoir installé la biblio graphicsmagick)
$(TARGET): src/objects.hpp src/raytrace.hpp src/tga_image.hpp
	$(CC) $(CFLAGS) -o $(TARGET) src/$(TARGET).cpp     
	
	./$(TARGET) scenes/scene1.txt image1.tga
	./$(TARGET) scenes/scene2.txt image2.tga
	./$(TARGET) scenes/scene3.txt image3.tga
	./$(TARGET) scenes/scene4.txt image4.tga

	convert image1.tga image1.png
	convert image2.tga image2.png
	convert image3.tga image3.png
	convert image4.tga image4.png

	$(RM) image1.tga
	$(RM) image2.tga
	$(RM) image3.tga
	$(RM) image4.tga

	mv image1.png images/image1.png
	mv image2.png images/image2.png
	mv image3.png images/image3.png
	mv image4.png images/image4.png

	eog images/image1.png
	eog images/image2.png
	eog images/image3.png
	eog images/image4.png

clean:
	$(RM) $(TARGET)
	$(RM) images/*.png

image3:	
	$(CC) $(CFLAGS) -o $(TARGET) src/$(TARGET).cpp
	./$(TARGET) scenes/scene3.txt image3.tga
	convert image3.tga image3.png
	$(RM) image3.tga
	mv image3.png images/image3.png
	eog images/image3.png