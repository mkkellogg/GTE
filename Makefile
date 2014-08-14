LIBS=-lm -lGL -lglut -lGLU -lGLEW
CFLAGS=-I src -std=c++11 -Wall
CC=g++

BASESRC = src/base
GTEMATHSRC = src/gtemath
GEOMETRYSRC= src/geometry
OBJECTSRC= src/graphics/object
UTILSRC= src/util
GRAPHICSSRC= src/graphics
SHADERSRC= $(GRAPHICSSRC)/shader

BASEOBJ= obj/basevector4.o obj/basevector2.o
GTEMAINOBJ= obj/gte.o
GTEMATHOBJ= obj/gtemath.o
GEOMETRYOBJ= obj/matrix.o obj/quaternion.o obj/point3.o obj/vector3.o obj/vector3array.o obj/point3array.o
OBJECTOBJ= obj/mesh3D.o 
UIOBJ= obj/debug.o 
GRAPHICSOBJ= obj/graphics.o obj/vertexattrbuffer.o obj/color4.o obj/uv2.o
SHADEROBJ= obj/shadersource.o obj/shader.o 

OPENGLOBJ= obj/graphicsGL.o obj/shaderGL.o obj/vertexattrbufferGL.o

OBJECTFILES= $(BASEOBJ) $(GTEMAINOBJ) $(GTEMATHOBJ) $(GEOMETRYOBJ) $(OBJECTOBJ) $(UIOBJ) $(GRAPHICSOBJ) $(SHADEROBJ) $(OPENGLOBJ)

all: gtemain graphics ui geometry gtemath object base
	$(CC) -o bin/gte $(OBJECTFILES) $(LIBS) 
	rm -rf bin/resources
	cp -r resources bin/



# ==================================
# GTE
# ==================================

gtemain: $(GTEMAINOBJ)
 
obj/gte.o: src/gte.cpp src/gte.h
	$(CC) $(CFLAGS) -o obj/gte.o -c src/gte.cpp

# ==================================
# Graphics
# ==================================

graphics: $(GRAPHICSOBJ) $(SHADEROBJ) $(OPENGLOBJ)

obj/graphics.o: $(GRAPHICSSRC)/graphics.cpp  $(GRAPHICSSRC)/graphics.h 
	$(CC) $(CFLAGS) -o obj/graphics.o -c $(GRAPHICSSRC)/graphics.cpp
	
obj/vertexattrbuffer.o:  $(GRAPHICSSRC)/vertexattrbuffer.cpp  $(GRAPHICSSRC)/vertexattrbuffer.h
	$(CC) $(CFLAGS) -o obj/vertexattrbuffer.o -c $(GRAPHICSSRC)/vertexattrbuffer.cpp

obj/shadersource.o: $(SHADERSRC)/shadersource.cpp  $(SHADERSRC)/shadersource.h 
	$(CC) $(CFLAGS) -o obj/shadersource.o -c $(SHADERSRC)/shadersource.cpp
	
obj/shader.o: $(SHADERSRC)/shader.cpp $(SHADERSRC)/shader.h 
	$(CC) $(CFLAGS) -o obj/shader.o -c $(SHADERSRC)/shader.cpp

obj/graphicsGL.o: $(GRAPHICSSRC)/graphicsGL.cpp  $(GRAPHICSSRC)/graphicsGL.h
	$(CC) $(CFLAGS) -o obj/graphicsGL.o -c $(GRAPHICSSRC)/graphicsGL.cpp
		
obj/shaderGL.o: $(SHADERSRC)/shaderGL.cpp $(SHADERSRC)/shaderGL.h
	$(CC) $(CFLAGS) -o obj/shaderGL.o -c $(SHADERSRC)/shaderGL.cpp

obj/vertexattrbufferGL.o:  $(GRAPHICSSRC)/vertexattrbufferGL.cpp  $(GRAPHICSSRC)/vertexattrbufferGL.h
	$(CC) $(CFLAGS) -o obj/vertexattrbufferGL.o -c $(GRAPHICSSRC)/vertexattrbufferGL.cpp
	
obj/color4.o: $(GRAPHICSSRC)/color4.cpp $(GRAPHICSSRC)/color4.h
	$(CC) $(CFLAGS) -o obj/color4.o -c $(GRAPHICSSRC)/color4.cpp
	
obj/uv2.o: $(GRAPHICSSRC)/uv2.cpp $(GRAPHICSSRC)/uv2.h
	$(CC) $(CFLAGS) -o obj/uv2.o -c $(GRAPHICSSRC)/uv2.cpp
	
# ==================================
# UI
# ==================================	

ui: $(UIOBJ)

obj/debug.o: src/ui/debug.cpp src/ui/debug.h
	$(CC) $(CFLAGS) -o obj/debug.o -c src/ui/debug.cpp 


# ==================================
# Geometry
# ==================================

geometry: $(GEOMETRYOBJ)

obj/point3.o: $(GEOMETRYSRC)/point3.cpp $(GEOMETRYSRC)/point3.h 
	$(CC) $(CFLAGS) -o obj/point3.o -c $(GEOMETRYSRC)/point3.cpp
	
obj/vector3.o: $(GEOMETRYSRC)/vector3.cpp $(GEOMETRYSRC)/vector3.h 
	$(CC) $(CFLAGS) -o obj/vector3.o -c $(GEOMETRYSRC)/vector3.cpp
	
obj/vector3array.o: $(GEOMETRYSRC)/vector3array.cpp $(GEOMETRYSRC)/vector3array.h 
	$(CC) $(CFLAGS) -o obj/vector3array.o -c $(GEOMETRYSRC)/vector3array.cpp
	
obj/point3array.o: $(GEOMETRYSRC)/point3array.cpp $(GEOMETRYSRC)/point3array.h 
	$(CC) $(CFLAGS) -o obj/point3array.o -c $(GEOMETRYSRC)/point3array.cpp
	
obj/matrix.o: $(GEOMETRYSRC)/matrix.cpp $(GEOMETRYSRC)/matrix.h 
	$(CC) $(CFLAGS) -o obj/matrix.o -c $(GEOMETRYSRC)/matrix.cpp
	
obj/quaternion.o: $(GEOMETRYSRC)/quaternion.cpp $(GEOMETRYSRC)/quaternion.h 
	$(CC) $(CFLAGS) -o obj/quaternion.o -c $(GEOMETRYSRC)/quaternion.cpp


# ==================================
# Base
# ==================================

base: $(BASEOBJ)

obj/basevector4.o: $(BASESRC)/basevector4.cpp $(BASESRC)/basevector4.h 
	$(CC) $(CFLAGS) -o obj/basevector4.o -c $(BASESRC)/basevector4.cpp
	
obj/basevector2.o: $(BASESRC)/basevector2.cpp $(BASESRC)/basevector2.h 
	$(CC) $(CFLAGS) -o obj/basevector2.o -c $(BASESRC)/basevector2.cpp

# ==================================
# Object
# ==================================

object: $(OBJECTOBJ)

obj/mesh3D.o: $(OBJECTSRC)/mesh3D.cpp $(OBJECTSRC)/mesh3D.h
	$(CC) $(CFLAGS) -o obj/mesh3D.o -c $(OBJECTSRC)/mesh3D.cpp
	

# ==================================
# Math
# ==================================

gtemath: $(GTEMATHOBJ)

obj/gtemath.o: $(GTEMATHSRC)/gtemath.cpp $(GTEMATHSRC)/gtemath.h
	$(CC) $(CFLAGS) -o obj/gtemath.o -c $(GTEMATHSRC)/gtemath.cpp


#util: $(UTILSRC)/varproxy.cpp $(UTILSRC)/varproxy.h
#	$(CC) $(CFLAGS) -o obj/varproxy.o -c $(UTILSRC)/varproxy.cpp

clean:
	rm -f obj/*   
	rm -f bin/gte
	rm -rf bin/resources
