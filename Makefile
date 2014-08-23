LIBS=-lm -lGL -lglut -lGLU -lGLEW
CFLAGS=-I src -std=c++11 -Wall
CC=g++

GLOBALSRC = src/global
BASESRC = src/base
GTEMATHSRC = src/gtemath
GEOMETRYSRC= src/geometry
UTILSRC= src/util
GRAPHICSSRC= src/graphics
SHADERSRC= $(GRAPHICSSRC)/shader
VIEWSYSSRC= $(GRAPHICSSRC)/view
RENDERSRC= $(GRAPHICSSRC)/render
GRAPHICSOBJECTSRC= $(GRAPHICSSRC)/object
ENGINEOBJECTSRC= src/object

GLOBALOBJ = obj/constants.o
BASEOBJ= obj/basevector4.o obj/basevector2.o obj/basevector2factory.o obj/basevector4factory.o obj/basevector2array.o obj/basevector4array.o obj/intmask.o
GTEMAINOBJ= obj/gte.o
GTEMATHOBJ= obj/gtemath.o
GEOMETRYOBJ= obj/matrix4x4.o obj/quaternion.o obj/point3.o obj/vector3.o obj/vector3factory.o obj/point3factory.o obj/vector3array.o obj/point3array.o obj/transform.o
GRAPHICSOBJECTOBJ= obj/mesh3D.o 
UIOBJ= obj/debug.o 
VIEWSYSOBJ= obj/viewsystem.o obj/camera.o 
RENDEROBJ= obj/mesh3Drenderer.o obj/renderbuffer.o obj/vertexattrbuffer.o obj/material.o obj/rendermanager.o
GRAPHICSOBJ= obj/graphics.o obj/color4.o obj/color4factory.o obj/color4array.o obj/uv2.o obj/uv2factory.o obj/uv2array.o obj/attributes.o obj/uniforms.o
SHADEROBJ= obj/shadersource.o obj/shader.o 
ENGINEOBJECTOBJ= obj/sceneobjectcomponent.o obj/engineobjectmanager.o obj/engineobject.o obj/sceneobject.o

OPENGLOBJ= obj/graphicsGL.o obj/shaderGL.o obj/vertexattrbufferGL.o obj/mesh3DrendererGL.o 

OBJECTFILES= $(BASEOBJ) $(GTEMAINOBJ) $(GTEMATHOBJ) $(GEOMETRYOBJ) $(GRAPHICSOBJECTOBJ) $(UIOBJ) $(GRAPHICSOBJ) $(VIEWSYSOBJ) $(RENDEROBJ) $(SHADEROBJ) $(OPENGLOBJ) $(GLOBALOBJ) $(ENGINEOBJECTOBJ)

all: gtemain graphics ui geometry gtemath base global engineobjects
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
#Glboal
# ==================================	

global: $(GLOBALOBJ)

obj/constants.o: $(GLOBALSRC)/constants.cpp $(GLOBALSRC)/constants.h
	$(CC) $(CFLAGS) -o obj/constants.o -c $(GLOBALSRC)/constants.cpp 
	
	
# ==================================
# EngineObjects
# ==================================

engineobjects: $(ENGINEOBJECTOBJ)
	
obj/sceneobjectcomponent.o: $(ENGINEOBJECTSRC)/sceneobjectcomponent.cpp $(ENGINEOBJECTSRC)/sceneobjectcomponent.h 
	$(CC) $(CFLAGS) -o obj/sceneobjectcomponent.o -c $(ENGINEOBJECTSRC)/sceneobjectcomponent.cpp
	
obj/sceneobject.o: $(ENGINEOBJECTSRC)/sceneobject.cpp $(ENGINEOBJECTSRC)/sceneobject.h 
	$(CC) $(CFLAGS) -o obj/sceneobject.o -c $(ENGINEOBJECTSRC)/sceneobject.cpp
	
obj/engineobject.o: $(ENGINEOBJECTSRC)/engineobject.cpp $(ENGINEOBJECTSRC)/engineobject.h 
	$(CC) $(CFLAGS) -o obj/engineobject.o -c $(ENGINEOBJECTSRC)/engineobject.cpp
	
obj/engineobjectmanager.o: $(ENGINEOBJECTSRC)/engineobjectmanager.cpp $(ENGINEOBJECTSRC)/engineobjectmanager.h 
	$(CC) $(CFLAGS) -o obj/engineobjectmanager.o -c $(ENGINEOBJECTSRC)/engineobjectmanager.cpp
	
	
	
# ==================================
# Graphics
# ==================================


graphics: $(GRAPHICSOBJ) $(SHADEROBJ) $(OPENGLOBJ) $(VIEWSYSOBJ) render graphicsobject

obj/graphics.o: $(GRAPHICSSRC)/graphics.cpp  $(GRAPHICSSRC)/graphics.h 
	$(CC) $(CFLAGS) -o obj/graphics.o -c $(GRAPHICSSRC)/graphics.cpp
	
obj/viewsystem.o: $(GRAPHICSSRC)/view/viewsystem.cpp  $(GRAPHICSSRC)/view/viewsystem.h 
	$(CC) $(CFLAGS) -o obj/viewsystem.o -c $(GRAPHICSSRC)/view/viewsystem.cpp
	
obj/camera.o: $(GRAPHICSSRC)/view/camera.cpp  $(GRAPHICSSRC)/view/camera.h 
	$(CC) $(CFLAGS) -o obj/camera.o -c $(GRAPHICSSRC)/view/camera.cpp

obj/shadersource.o: $(SHADERSRC)/shadersource.cpp  $(SHADERSRC)/shadersource.h 
	$(CC) $(CFLAGS) -o obj/shadersource.o -c $(SHADERSRC)/shadersource.cpp
	
obj/shader.o: $(SHADERSRC)/shader.cpp $(SHADERSRC)/shader.h 
	$(CC) $(CFLAGS) -o obj/shader.o -c $(SHADERSRC)/shader.cpp

obj/graphicsGL.o: $(GRAPHICSSRC)/graphicsGL.cpp  $(GRAPHICSSRC)/graphicsGL.h
	$(CC) $(CFLAGS) -o obj/graphicsGL.o -c $(GRAPHICSSRC)/graphicsGL.cpp
		
obj/shaderGL.o: $(SHADERSRC)/shaderGL.cpp $(SHADERSRC)/shaderGL.h
	$(CC) $(CFLAGS) -o obj/shaderGL.o -c $(SHADERSRC)/shaderGL.cpp
	
obj/color4.o: $(GRAPHICSSRC)/color/color4.cpp $(GRAPHICSSRC)/color/color4.h
	$(CC) $(CFLAGS) -o obj/color4.o -c $(GRAPHICSSRC)/color/color4.cpp

obj/color4factory.o: $(GRAPHICSSRC)/color/color4factory.cpp $(GRAPHICSSRC)/color/color4factory.h
	$(CC) $(CFLAGS) -o obj/color4factory.o -c $(GRAPHICSSRC)/color/color4factory.cpp

obj/color4array.o: $(GRAPHICSSRC)/color/color4array.cpp $(GRAPHICSSRC)/color/color4array.h
	$(CC) $(CFLAGS) -o obj/color4array.o -c $(GRAPHICSSRC)/color/color4array.cpp
	
obj/uv2.o: $(GRAPHICSSRC)/uv/uv2.cpp $(GRAPHICSSRC)/uv/uv2.h
	$(CC) $(CFLAGS) -o obj/uv2.o -c $(GRAPHICSSRC)/uv/uv2.cpp
	
obj/uv2factory.o: $(GRAPHICSSRC)/uv/uv2factory.cpp $(GRAPHICSSRC)/uv/uv2factory.h
	$(CC) $(CFLAGS) -o obj/uv2factory.o -c $(GRAPHICSSRC)/uv/uv2factory.cpp
	
obj/uv2array.o: $(GRAPHICSSRC)/uv/uv2array.cpp $(GRAPHICSSRC)/uv/uv2array.h
	$(CC) $(CFLAGS) -o obj/uv2array.o -c $(GRAPHICSSRC)/uv/uv2array.cpp

obj/attributes.o: $(GRAPHICSSRC)/attributes.cpp $(GRAPHICSSRC)/attributes.h
	$(CC) $(CFLAGS) -o obj/attributes.o -c $(GRAPHICSSRC)/attributes.cpp
	
obj/uniforms.o: $(GRAPHICSSRC)/uniforms.cpp $(GRAPHICSSRC)/uniforms.h
	$(CC) $(CFLAGS) -o obj/uniforms.o -c $(GRAPHICSSRC)/uniforms.cpp
	
	
# ==================================
# Render
# ==================================

render: $(RENDEROBJ)

obj/rendermanager.o: $(RENDERSRC)/rendermanager.cpp $(RENDERSRC)/rendermanager.h
	$(CC) $(CFLAGS) -o obj/rendermanager.o -c $(RENDERSRC)/rendermanager.cpp
	
obj/mesh3Drenderer.o: $(RENDERSRC)/mesh3Drenderer.cpp $(RENDERSRC)/mesh3Drenderer.h
	$(CC) $(CFLAGS) -o obj/mesh3Drenderer.o -c $(RENDERSRC)/mesh3Drenderer.cpp
	
obj/mesh3DrendererGL.o: $(RENDERSRC)/mesh3DrendererGL.cpp $(RENDERSRC)/mesh3DrendererGL.h
	$(CC) $(CFLAGS) -o obj/mesh3DrendererGL.o -c $(RENDERSRC)/mesh3DrendererGL.cpp
	
obj/material.o: $(RENDERSRC)/material.cpp $(RENDERSRC)/material.h
	$(CC) $(CFLAGS) -o obj/material.o -c $(RENDERSRC)/material.cpp
	
obj/vertexattrbufferGL.o:  $(RENDERSRC)/vertexattrbufferGL.cpp  $(RENDERSRC)/vertexattrbufferGL.h
	$(CC) $(CFLAGS) -o obj/vertexattrbufferGL.o -c $(RENDERSRC)/vertexattrbufferGL.cpp
	
obj/renderbuffer.o: $(RENDERSRC)/renderbuffer.cpp  $(RENDERSRC)/renderbuffer.h 
	$(CC) $(CFLAGS) -o obj/renderbuffer.o -c $(RENDERSRC)/renderbuffer.cpp
	
obj/vertexattrbuffer.o:  $(RENDERSRC)/vertexattrbuffer.cpp  $(RENDERSRC)/vertexattrbuffer.h
	$(CC) $(CFLAGS) -o obj/vertexattrbuffer.o -c $(RENDERSRC)/vertexattrbuffer.cpp


# ==================================
# GraphicsObject
# ==================================

graphicsobject: $(GRAPHICSOBJECTOBJ)
	
obj/mesh3D.o: $(GRAPHICSOBJECTSRC)/mesh3D.cpp $(GRAPHICSOBJECTSRC)/mesh3D.h 
	$(CC) $(CFLAGS) -o obj/mesh3D.o -c $(GRAPHICSOBJECTSRC)/mesh3D.cpp
	
	
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
	
obj/transform.o: $(GEOMETRYSRC)/transform.cpp $(GEOMETRYSRC)/transform.h 
	$(CC) $(CFLAGS) -o obj/transform.o -c $(GEOMETRYSRC)/transform.cpp
	
obj/vector3.o: $(GEOMETRYSRC)/vector/vector3.cpp $(GEOMETRYSRC)/vector/vector3.h 
	$(CC) $(CFLAGS) -o obj/vector3.o -c $(GEOMETRYSRC)/vector/vector3.cpp
	
obj/vector3factory.o: $(GEOMETRYSRC)/vector/vector3factory.cpp $(GEOMETRYSRC)/vector/vector3factory.h 
	$(CC) $(CFLAGS) -o obj/vector3factory.o -c $(GEOMETRYSRC)/vector/vector3factory.cpp
	
obj/vector3array.o: $(GEOMETRYSRC)/vector/vector3array.cpp $(GEOMETRYSRC)/vector/vector3array.h 
	$(CC) $(CFLAGS) -o obj/vector3array.o -c $(GEOMETRYSRC)/vector/vector3array.cpp

obj/point3.o: $(GEOMETRYSRC)/point/point3.cpp $(GEOMETRYSRC)/point/point3.h 
	$(CC) $(CFLAGS) -o obj/point3.o -c $(GEOMETRYSRC)/point/point3.cpp

obj/point3factory.o: $(GEOMETRYSRC)/point/point3factory.cpp $(GEOMETRYSRC)/point/point3factory.h 
	$(CC) $(CFLAGS) -o obj/point3factory.o -c $(GEOMETRYSRC)/point/point3factory.cpp
	
obj/point3array.o: $(GEOMETRYSRC)/point/point3array.cpp $(GEOMETRYSRC)/point/point3array.h 
	$(CC) $(CFLAGS) -o obj/point3array.o -c $(GEOMETRYSRC)/point/point3array.cpp
	
obj/matrix4x4.o: $(GEOMETRYSRC)/matrix4x4.cpp $(GEOMETRYSRC)/matrix4x4.h 
	$(CC) $(CFLAGS) -o obj/matrix4x4.o -c $(GEOMETRYSRC)/matrix4x4.cpp
	
obj/quaternion.o: $(GEOMETRYSRC)/quaternion.cpp $(GEOMETRYSRC)/quaternion.h 
	$(CC) $(CFLAGS) -o obj/quaternion.o -c $(GEOMETRYSRC)/quaternion.cpp


# ==================================
# Base
# ==================================

base: $(BASEOBJ)

obj/intmask.o: $(BASESRC)/intmask.cpp $(BASESRC)/intmask.h 
	$(CC) $(CFLAGS) -o obj/intmask.o -c $(BASESRC)/intmask.cpp
	
obj/basevector4.o: $(BASESRC)/basevector4.cpp $(BASESRC)/basevector4.h 
	$(CC) $(CFLAGS) -o obj/basevector4.o -c $(BASESRC)/basevector4.cpp

obj/basevector4array.o: $(BASESRC)/basevector4array.cpp $(BASESRC)/basevector4array.h 
	$(CC) $(CFLAGS) -o obj/basevector4array.o -c $(BASESRC)/basevector4array.cpp
	
obj/basevector4factory.o: $(BASESRC)/basevector4factory.cpp $(BASESRC)/basevector4factory.h 
	$(CC) $(CFLAGS) -o obj/basevector4factory.o -c $(BASESRC)/basevector4factory.cpp
	
obj/basevector2.o: $(BASESRC)/basevector2.cpp $(BASESRC)/basevector2.h 
	$(CC) $(CFLAGS) -o obj/basevector2.o -c $(BASESRC)/basevector2.cpp
	
obj/basevector2factory.o: $(BASESRC)/basevector2factory.cpp $(BASESRC)/basevector2factory.h 
	$(CC) $(CFLAGS) -o obj/basevector2factory.o -c $(BASESRC)/basevector2factory.cpp

obj/basevector2array.o: $(BASESRC)/basevector2array.cpp $(BASESRC)/basevector2array.h 
	$(CC) $(CFLAGS) -o obj/basevector2array.o -c $(BASESRC)/basevector2array.cpp
	

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
