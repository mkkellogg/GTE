LIBS=-lm -lGL -lglut -lGLU -lGLEW
CFLAGS=-I src -std=c++11 -Wall
CC=g++
OBJECTFILES= obj/shadermanager.o obj/shadersource.o obj/shader.o obj/graphics.o obj/debug.o obj/matrix.o obj/quaternion.o obj/point3.o obj/vector3.o obj/basevector3.o obj/mesh3D.o obj/gtemath.o obj/gte.o
PLATFORMOBJECTFILES= obj/graphicsGL.o obj/shaderGL.o

GTEMATHSRC = src/gtemath
GEOMETRYSRC= src/geometry
UTILSRC= src/util
GRAPHICSSRC= src/graphics
SHADERSRC= $(GRAPHICSSRC)/shader

all: gtemain graphics ui geometry gtemath
	$(CC) -o bin/gte $(OBJECTFILES) $(PLATFORMOBJECTFILES) $(LIBS) 
	rm -rf bin/resources
	cp -r resources bin/

gtemain: src/gte.cpp src/gte.h
	$(CC) $(CFLAGS) -o obj/gte.o -c src/gte.cpp

graphics: shader $(GRAPHICSSRC)/graphics.cpp  $(GRAPHICSSRC)/graphics.h $(GRAPHICSSRC)/graphicsGL.cpp  $(GRAPHICSSRC)/graphicsGL.h
	$(CC) $(CFLAGS) -o obj/graphics.o -c $(GRAPHICSSRC)/graphics.cpp
	$(CC) $(CFLAGS) -o obj/graphicsGL.o -c $(GRAPHICSSRC)/graphicsGL.cpp

shader: $(SHADERSRC)/shadermanager.cpp $(SHADERSRC)/shadermanager.h $(SHADERSRC)/shadersource.cpp  $(SHADERSRC)/shadersource.h $(SHADERSRC)/shader.cpp $(SHADERSRC)/shader.h $(SHADERSRC)/shaderGL.cpp $(SHADERSRC)/shaderGL.h
	$(CC) $(CFLAGS) -o obj/shadermanager.o -c $(SHADERSRC)/shadermanager.cpp 
	$(CC) $(CFLAGS) -o obj/shadersource.o -c $(SHADERSRC)/shadersource.cpp
	$(CC) $(CFLAGS) -o obj/shader.o -c $(SHADERSRC)/shader.cpp
	$(CC) $(CFLAGS) -o obj/shaderGL.o -c $(SHADERSRC)/shaderGL.cpp

ui: src/ui/debug.cpp src/ui/debug.h
	$(CC) $(CFLAGS) -o obj/debug.o -c src/ui/debug.cpp 

geometry: $(GEOMETRYSRC)/point3.cpp $(GEOMETRYSRC)/point3.h $(GEOMETRYSRC)/vector3.cpp $(GEOMETRYSRC)/vector3.h $(GEOMETRYSRC)/matrix.cpp $(GEOMETRYSRC)/matrix.h $(GEOMETRYSRC)/basevector3.cpp $(GEOMETRYSRC)/basevector3.h $(GEOMETRYSRC)/quaternion.cpp $(GEOMETRYSRC)/quaternion.h $(GEOMETRYSRC)/mesh3D.cpp $(GEOMETRYSRC)/mesh3D.h
	$(CC) $(CFLAGS) -o obj/matrix.o -c $(GEOMETRYSRC)/matrix.cpp
	$(CC) $(CFLAGS) -o obj/quaternion.o -c $(GEOMETRYSRC)/quaternion.cpp
	$(CC) $(CFLAGS) -o obj/point3.o -c $(GEOMETRYSRC)/point3.cpp
	$(CC) $(CFLAGS) -o obj/vector3.o -c $(GEOMETRYSRC)/vector3.cpp
	$(CC) $(CFLAGS) -o obj/basevector3.o -c $(GEOMETRYSRC)/basevector3.cpp
	$(CC) $(CFLAGS) -o obj/mesh3D.o -c $(GEOMETRYSRC)/mesh3D.cpp

gtemath: $(GTEMATHSRC)/gtemath.cpp $(GTEMATHSRC)/gtemath.h
	$(CC) $(CFLAGS) -o obj/gtemath.o -c $(GTEMATHSRC)/gtemath.cpp

#util: $(UTILSRC)/varproxy.cpp $(UTILSRC)/varproxy.h
#	$(CC) $(CFLAGS) -o obj/varproxy.o -c $(UTILSRC)/varproxy.cpp

clean:
	rm -f obj/*   
	rm -f bin/gte
	rm -rf bin/resources
