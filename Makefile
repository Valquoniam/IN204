# the compiler: gcc for C program, define as g++ for C++
CC = g++
 
# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -g -Wall
RM = rm

# The scene
SCENE = scene1.txt 

# The output image
OUTPUT = image1 image2

# The build target 
TARGET = raytrace

all: $(TARGET)
 
# Démarche :
#	- On compile raytrace.cpp
#	- On éxécute raytrace.exe avec comme arguments la scène (en 1ère) et l'image output (en 2ème)
#	- On convertit l'image .tga en .png (Il faut avoir installé la biblio graphicsmagick)
$(TARGET): 
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp     
	
	./$(TARGET) scenes/scene1.txt image1.tga
	./$(TARGET) scenes/scene2.txt image2.tga
	convert image1.tga image1.png
	convert image2.tga image2.png
	$(RM) image1.tga
	$(RM) image2.tga
	mv image1.png images/image1.png
	mv image2.png images/image2.png
	eog images/image1.png
	eog images/image2.png
 
clean:
	$(RM) $(TARGET)
	$(RM) images/*.png
