ASSIMP_LIB=../assimp/lib
ASSIMP_INC=../assimp/include
DEVIL_LIB=/usr/locl/lib
DEVIL_INC=../DevIL/DevIL/include
OPENGL_LIB=/usr/lib/nvidia-331-updates/ #this needs to be changed to your own OpenGL lib path

LIBS= -L$(OPENGL_LIB) -L$(ASSIMP_LIB) -L$(DEVIL_LIB) -lassimp -lm -lGL -lglut -lGLU -lGLEW -lassimp -lIL 
CFLAGS=-Isrc -I$(ASSIMP_INC) -I$(DEVIL_INC)  -std=c++11 -Wall 
CC=g++

GLOBALSRC = src/global
BASESRC = src/base
GTEMATHSRC = src/gtemath
GEOMETRYSRC= src/geometry
UTILSRC= src/util
GRAPHICSSRC= src/graphics
LIGHTSRC= $(GRAPHICSSRC)/light
SHADERSRC= $(GRAPHICSSRC)/shader
TEXTURESRC= $(GRAPHICSSRC)/texture
VIEWSYSSRC= $(GRAPHICSSRC)/view
RENDERSRC= $(GRAPHICSSRC)/render
GRAPHICSOBJECTSRC= $(GRAPHICSSRC)/object
ANIMATIONSRC= $(GRAPHICSSRC)/animation
IMAGESRC= $(GRAPHICSSRC)/image
ENGINEOBJECTSRC= src/object
ASSETSRC= src/asset
UTILSRC= src/util
FILESYSTEMSRC= src/filesys


GLOBALOBJ = obj/constants.o
BASEOBJ= obj/basevector4.o obj/basevector2.o obj/basevector2factory.o obj/basevector4factory.o obj/basevector2array.o obj/basevector4array.o obj/intmask.o obj/longmask.o
GTEMAINOBJ= obj/gte.o
GTEMATHOBJ= obj/gtemath.o
GEOMETRYOBJ= obj/matrix4x4.o obj/quaternion.o obj/point3.o obj/vector3.o obj/vector3factory.o obj/point3factory.o obj/vector3array.o obj/point3array.o obj/transform.o obj/sceneobjecttransform.o
GRAPHICSOBJECTOBJ= obj/mesh3D.o obj/submesh3D.o 
UIOBJ= obj/debug.o 
VIEWSYSOBJ= obj/camera.o 
RENDEROBJ= obj/mesh3Drenderer.o obj/skinnedmesh3Drenderer.o obj/submesh3Drenderer.o obj/attributetransformer.o obj/skinnedmesh3Dattrtransformer.o obj/renderbuffer.o obj/vertexattrbuffer.o obj/material.o obj/rendermanager.o 
GRAPHICSOBJ= obj/graphics.o obj/color4.o obj/color4factory.o obj/color4array.o obj/uv2.o obj/uv2factory.o obj/uv2array.o obj/stdattributes.o obj/stduniforms.o obj/screendesc.o 
ANIMATIONOBJ= obj/skeleton.o obj/vertexbonemap.o obj/skeletonnode.o obj/bone.o obj/sceneobjectskeletonnode.o obj/keyframeset.o obj/keyframe.o obj/translationkeyframe.o obj/scalekeyframe.o obj/rotationkeyframe.o obj/animation.o obj/animationmanager.o obj/animationinstance.o
LIGHTOBJ= obj/light.o
SHADEROBJ= obj/shadersource.o obj/shader.o obj/uniformdesc.o obj/attributedesc.o 
TEXTUREOBJ= obj/texture.o obj/textureattr.o  
IMAGEOBJ= obj/lodepng.o obj/lodepng_util.o obj/rawimage.o obj/imageloader.o
ENGINEOBJECTOBJ= obj/sceneobjectcomponent.o obj/engineobjectmanager.o obj/engineobject.o obj/sceneobject.o obj/shadermanager.o
ASSETOBJ= obj/assetimporter.o obj/importutil.o obj/modelimporter.o
UTILOBJ= obj/datastack.o obj/util.o obj/time.o
FILESYSTEMOBJ= obj/filesystem.o obj/filesystemIX.o

OPENGLOBJ= obj/graphicsGL.o obj/shaderGL.o obj/vertexattrbufferGL.o obj/submesh3DrendererGL.o obj/textureGL.o

OBJECTFILES= $(BASEOBJ) $(UTILOBJ) $(GTEMAINOBJ) $(GTEMATHOBJ) $(GEOMETRYOBJ) $(GRAPHICSOBJECTOBJ) $(UIOBJ) $(GRAPHICSOBJ) $(ANIMATIONOBJ) $(LIGHTOBJ) $(IMAGEOBJ) $(VIEWSYSOBJ) $(RENDEROBJ) $(SHADEROBJ) $(TEXTUREOBJ) $(OPENGLOBJ) $(GLOBALOBJ) $(ENGINEOBJECTOBJ) $(ASSETOBJ) $(FILESYSTEMOBJ)

all: gtemain graphics animation ui geometry gtemath base global engineobjects asset util image filesystem
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
# Global
# ==================================	

global: $(GLOBALOBJ)

obj/constants.o: $(GLOBALSRC)/constants.cpp $(GLOBALSRC)/constants.h
	$(CC) $(CFLAGS) -o obj/constants.o -c $(GLOBALSRC)/constants.cpp 
	

# ==================================
# FileSystem
# ==================================	

filesystem: $(FILESYSTEMOBJ)

obj/filesystem.o: $(FILESYSTEMSRC)/filesystem.cpp $(FILESYSTEMSRC)/filesystem.h
	$(CC) $(CFLAGS) -o obj/filesystem.o -c $(FILESYSTEMSRC)/filesystem.cpp 
	
obj/filesystemIX.o: $(FILESYSTEMSRC)/filesystemIX.cpp $(FILESYSTEMSRC)/filesystemIX.h
	$(CC) $(CFLAGS) -o obj/filesystemIX.o -c $(FILESYSTEMSRC)/filesystemIX.cpp 
	
	
# ==================================
# EngineObjects
# ==================================

engineobjects: $(ENGINEOBJECTOBJ)
	
obj/shadermanager.o: $(ENGINEOBJECTSRC)/shadermanager.cpp $(ENGINEOBJECTSRC)/shadermanager.h 
	$(CC) $(CFLAGS) -o obj/shadermanager.o -c $(ENGINEOBJECTSRC)/shadermanager.cpp
	
obj/sceneobjectcomponent.o: $(ENGINEOBJECTSRC)/sceneobjectcomponent.cpp $(ENGINEOBJECTSRC)/sceneobjectcomponent.h 
	$(CC) $(CFLAGS) -o obj/sceneobjectcomponent.o -c $(ENGINEOBJECTSRC)/sceneobjectcomponent.cpp
	
obj/sceneobject.o: $(ENGINEOBJECTSRC)/sceneobject.cpp $(ENGINEOBJECTSRC)/sceneobject.h 
	$(CC) $(CFLAGS) -o obj/sceneobject.o -c $(ENGINEOBJECTSRC)/sceneobject.cpp
	
obj/engineobject.o: $(ENGINEOBJECTSRC)/engineobject.cpp $(ENGINEOBJECTSRC)/engineobject.h 
	$(CC) $(CFLAGS) -o obj/engineobject.o -c $(ENGINEOBJECTSRC)/engineobject.cpp
	
obj/engineobjectmanager.o: $(ENGINEOBJECTSRC)/engineobjectmanager.cpp $(ENGINEOBJECTSRC)/engineobjectmanager.h 
	$(CC) $(CFLAGS) -o obj/engineobjectmanager.o -c $(ENGINEOBJECTSRC)/engineobjectmanager.cpp
	

# ==================================
# Asset
# ==================================

asset: $(ASSETOBJ)
	
obj/assetimporter.o: $(ASSETSRC)/assetimporter.cpp $(ASSETSRC)/assetimporter.h 
	$(CC) $(CFLAGS) -o obj/assetimporter.o -c $(ASSETSRC)/assetimporter.cpp
	
obj/modelimporter.o: $(ASSETSRC)/modelimporter.cpp $(ASSETSRC)/modelimporter.h 
	$(CC) $(CFLAGS) -o obj/modelimporter.o -c $(ASSETSRC)/modelimporter.cpp
	
obj/importutil.o: $(ASSETSRC)/importutil.cpp $(ASSETSRC)/importutil.h 
	$(CC) $(CFLAGS) -o obj/importutil.o -c $(ASSETSRC)/importutil.cpp
	
		
# ==================================
# Image processing
# ==================================	

image: $(IMAGEOBJ)

obj/lodepng.o: $(IMAGESRC)/lodepng/lodepng.cpp $(IMAGESRC)/lodepng/lodepng.h
	$(CC) $(CFLAGS) -o obj/lodepng.o -c $(IMAGESRC)/lodepng/lodepng.cpp 
	
obj/lodepng_util.o: $(IMAGESRC)/lodepng/lodepng_util.cpp $(IMAGESRC)/lodepng/lodepng_util.h
	$(CC) $(CFLAGS) -o obj/lodepng_util.o -c $(IMAGESRC)/lodepng/lodepng_util.cpp 
	
obj/rawimage.o: $(IMAGESRC)/rawimage.cpp $(IMAGESRC)/rawimage.h
	$(CC) $(CFLAGS) -o obj/rawimage.o -c $(IMAGESRC)/rawimage.cpp 
	
obj/imageloader.o: $(IMAGESRC)/imageloader.cpp $(IMAGESRC)/imageloader.h
	$(CC) $(CFLAGS) -o obj/imageloader.o -c $(IMAGESRC)/imageloader.cpp 
	
	
# ==================================
# Graphics
# ==================================


graphics: $(GRAPHICSOBJ) $(LIGHTOBJ) $(TEXTUREOBJ) $(OPENGLOBJ) $(VIEWSYSOBJ) render graphicsobject shader

obj/graphics.o: $(GRAPHICSSRC)/graphics.cpp  $(GRAPHICSSRC)/graphics.h 
	$(CC) $(CFLAGS) -o obj/graphics.o -c $(GRAPHICSSRC)/graphics.cpp

obj/graphicsGL.o: $(GRAPHICSSRC)/graphicsGL.cpp  $(GRAPHICSSRC)/graphicsGL.h
	$(CC) $(CFLAGS) -o obj/graphicsGL.o -c $(GRAPHICSSRC)/graphicsGL.cpp
	
obj/screendesc.o: $(GRAPHICSSRC)/screendesc.cpp  $(GRAPHICSSRC)/screendesc.h 
	$(CC) $(CFLAGS) -o obj/screendesc.o -c $(GRAPHICSSRC)/screendesc.cpp
	
obj/camera.o: $(GRAPHICSSRC)/view/camera.cpp  $(GRAPHICSSRC)/view/camera.h 
	$(CC) $(CFLAGS) -o obj/camera.o -c $(GRAPHICSSRC)/view/camera.cpp

obj/light.o: $(LIGHTSRC)/light.cpp $(LIGHTSRC)/light.h 
	$(CC) $(CFLAGS) -o obj/light.o -c $(LIGHTSRC)/light.cpp
	
obj/texture.o: $(TEXTURESRC)/texture.cpp $(TEXTURESRC)/texture.h 
	$(CC) $(CFLAGS) -o obj/texture.o -c $(TEXTURESRC)/texture.cpp
	
obj/textureattr.o: $(TEXTURESRC)/textureattr.cpp $(TEXTURESRC)/textureattr.h 
	$(CC) $(CFLAGS) -o obj/textureattr.o -c $(TEXTURESRC)/textureattr.cpp
	
obj/textureGL.o: $(TEXTURESRC)/textureGL.cpp $(TEXTURESRC)/textureGL.h 
	$(CC) $(CFLAGS) -o obj/textureGL.o -c $(TEXTURESRC)/textureGL.cpp

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

obj/stdattributes.o: $(GRAPHICSSRC)/stdattributes.cpp $(GRAPHICSSRC)/stdattributes.h
	$(CC) $(CFLAGS) -o obj/stdattributes.o -c $(GRAPHICSSRC)/stdattributes.cpp
	
obj/stduniforms.o: $(GRAPHICSSRC)/stduniforms.cpp $(GRAPHICSSRC)/stduniforms.h
	$(CC) $(CFLAGS) -o obj/stduniforms.o -c $(GRAPHICSSRC)/stduniforms.cpp
	

# ==================================
# Animation
# ==================================	

animation: $(ANIMATIONOBJ)

obj/skeleton.o: $(ANIMATIONSRC)/skeleton.cpp $(ANIMATIONSRC)/skeleton.h
	$(CC) $(CFLAGS) -o obj/skeleton.o -c $(ANIMATIONSRC)/skeleton.cpp 
	
obj/skeletonnode.o: $(ANIMATIONSRC)/skeletonnode.cpp $(ANIMATIONSRC)/skeletonnode.h
	$(CC) $(CFLAGS) -o obj/skeletonnode.o -c $(ANIMATIONSRC)/skeletonnode.cpp 
	
obj/bone.o: $(ANIMATIONSRC)/bone.cpp $(ANIMATIONSRC)/bone.h
	$(CC) $(CFLAGS) -o obj/bone.o -c $(ANIMATIONSRC)/bone.cpp
	
obj/sceneobjectskeletonnode.o: $(ANIMATIONSRC)/sceneobjectskeletonnode.cpp $(ANIMATIONSRC)/sceneobjectskeletonnode.h
	$(CC) $(CFLAGS) -o obj/sceneobjectskeletonnode.o -c $(ANIMATIONSRC)/sceneobjectskeletonnode.cpp 
		
 obj/vertexbonemap.o: $(ANIMATIONSRC)/vertexbonemap.cpp $(ANIMATIONSRC)/vertexbonemap.h
	$(CC) $(CFLAGS) -o obj/vertexbonemap.o -c $(ANIMATIONSRC)/vertexbonemap.cpp
	
 obj/keyframeset.o: $(ANIMATIONSRC)/keyframeset.cpp $(ANIMATIONSRC)/keyframeset.h
	$(CC) $(CFLAGS) -o obj/keyframeset.o -c $(ANIMATIONSRC)/keyframeset.cpp
	
 obj/keyframe.o: $(ANIMATIONSRC)/keyframe.cpp $(ANIMATIONSRC)/keyframe.h
	$(CC) $(CFLAGS) -o obj/keyframe.o -c $(ANIMATIONSRC)/keyframe.cpp
	
 obj/translationkeyframe.o: $(ANIMATIONSRC)/translationkeyframe.cpp $(ANIMATIONSRC)/translationkeyframe.h
	$(CC) $(CFLAGS) -o obj/translationkeyframe.o -c $(ANIMATIONSRC)/translationkeyframe.cpp
	
 obj/scalekeyframe.o: $(ANIMATIONSRC)/scalekeyframe.cpp $(ANIMATIONSRC)/scalekeyframe.h
	$(CC) $(CFLAGS) -o obj/scalekeyframe.o -c $(ANIMATIONSRC)/scalekeyframe.cpp
	
 obj/rotationkeyframe.o: $(ANIMATIONSRC)/rotationkeyframe.cpp $(ANIMATIONSRC)/rotationkeyframe.h
	$(CC) $(CFLAGS) -o obj/rotationkeyframe.o -c $(ANIMATIONSRC)/rotationkeyframe.cpp
	
 obj/animation.o: $(ANIMATIONSRC)/animation.cpp $(ANIMATIONSRC)/animation.h
	$(CC) $(CFLAGS) -o obj/animation.o -c $(ANIMATIONSRC)/animation.cpp

 obj/animationmanager.o: $(ANIMATIONSRC)/animationmanager.cpp $(ANIMATIONSRC)/animationmanager.h
	$(CC) $(CFLAGS) -o obj/animationmanager.o -c $(ANIMATIONSRC)/animationmanager.cpp	
	
obj/animationinstance.o: $(ANIMATIONSRC)/animationinstance.cpp $(ANIMATIONSRC)/animationinstance.h
	$(CC) $(CFLAGS) -o obj/animationinstance.o -c $(ANIMATIONSRC)/animationinstance.cpp	
	
	
# ==================================
# Shader
# ==================================	

shader: $(SHADEROBJ)

obj/shadersource.o: $(SHADERSRC)/shadersource.cpp  $(SHADERSRC)/shadersource.h 
	$(CC) $(CFLAGS) -o obj/shadersource.o -c $(SHADERSRC)/shadersource.cpp
	
obj/shader.o: $(SHADERSRC)/shader.cpp $(SHADERSRC)/shader.h 
	$(CC) $(CFLAGS) -o obj/shader.o -c $(SHADERSRC)/shader.cpp
		
obj/shaderGL.o: $(SHADERSRC)/shaderGL.cpp $(SHADERSRC)/shaderGL.h
	$(CC) $(CFLAGS) -o obj/shaderGL.o -c $(SHADERSRC)/shaderGL.cpp
	
obj/uniformdesc.o: $(SHADERSRC)/uniformdesc.cpp $(SHADERSRC)/uniformdesc.h
	$(CC) $(CFLAGS) -o obj/uniformdesc.o -c $(SHADERSRC)/uniformdesc.cpp
	
obj/attributedesc.o: $(SHADERSRC)/attributedesc.cpp $(SHADERSRC)/attributedesc.h
	$(CC) $(CFLAGS) -o obj/attributedesc.o -c $(SHADERSRC)/attributedesc.cpp

	
# ==================================
# Render
# ==================================

render: $(RENDEROBJ)

obj/rendermanager.o: $(RENDERSRC)/rendermanager.cpp $(RENDERSRC)/rendermanager.h
	$(CC) $(CFLAGS) -o obj/rendermanager.o -c $(RENDERSRC)/rendermanager.cpp
	
obj/mesh3Drenderer.o: $(RENDERSRC)/mesh3Drenderer.cpp $(RENDERSRC)/mesh3Drenderer.h
	$(CC) $(CFLAGS) -o obj/mesh3Drenderer.o -c $(RENDERSRC)/mesh3Drenderer.cpp
	
obj/skinnedmesh3Drenderer.o: $(RENDERSRC)/skinnedmesh3Drenderer.cpp $(RENDERSRC)/skinnedmesh3Drenderer.h
	$(CC) $(CFLAGS) -o obj/skinnedmesh3Drenderer.o -c $(RENDERSRC)/skinnedmesh3Drenderer.cpp
	
obj/submesh3Drenderer.o: $(RENDERSRC)/submesh3Drenderer.cpp $(RENDERSRC)/submesh3Drenderer.h
	$(CC) $(CFLAGS) -o obj/submesh3Drenderer.o -c $(RENDERSRC)/submesh3Drenderer.cpp
	
obj/submesh3DrendererGL.o: $(RENDERSRC)/submesh3DrendererGL.cpp $(RENDERSRC)/submesh3DrendererGL.h
	$(CC) $(CFLAGS) -o obj/submesh3DrendererGL.o -c $(RENDERSRC)/submesh3DrendererGL.cpp
	
obj/material.o: $(RENDERSRC)/material.cpp $(RENDERSRC)/material.h
	$(CC) $(CFLAGS) -o obj/material.o -c $(RENDERSRC)/material.cpp
	
obj/vertexattrbufferGL.o:  $(RENDERSRC)/vertexattrbufferGL.cpp  $(RENDERSRC)/vertexattrbufferGL.h
	$(CC) $(CFLAGS) -o obj/vertexattrbufferGL.o -c $(RENDERSRC)/vertexattrbufferGL.cpp
	
obj/renderbuffer.o: $(RENDERSRC)/renderbuffer.cpp  $(RENDERSRC)/renderbuffer.h 
	$(CC) $(CFLAGS) -o obj/renderbuffer.o -c $(RENDERSRC)/renderbuffer.cpp
	
obj/vertexattrbuffer.o:  $(RENDERSRC)/vertexattrbuffer.cpp  $(RENDERSRC)/vertexattrbuffer.h
	$(CC) $(CFLAGS) -o obj/vertexattrbuffer.o -c $(RENDERSRC)/vertexattrbuffer.cpp
	
obj/attributetransformer.o: $(RENDERSRC)/attributetransformer.cpp $(RENDERSRC)/attributetransformer.h
	$(CC) $(CFLAGS) -o obj/attributetransformer.o -c $(RENDERSRC)/attributetransformer.cpp
	
obj/skinnedmesh3Dattrtransformer.o: $(RENDERSRC)/skinnedmesh3Dattrtransformer.cpp $(RENDERSRC)/skinnedmesh3Dattrtransformer.h
	$(CC) $(CFLAGS) -o obj/skinnedmesh3Dattrtransformer.o -c $(RENDERSRC)/skinnedmesh3Dattrtransformer.cpp
	
	
# ==================================
# GraphicsObject
# ==================================

graphicsobject: $(GRAPHICSOBJECTOBJ)
	
obj/mesh3D.o: $(GRAPHICSOBJECTSRC)/mesh3D.cpp $(GRAPHICSOBJECTSRC)/mesh3D.h 
	$(CC) $(CFLAGS) -o obj/mesh3D.o -c $(GRAPHICSOBJECTSRC)/mesh3D.cpp
	
obj/submesh3D.o: $(GRAPHICSOBJECTSRC)/submesh3D.cpp $(GRAPHICSOBJECTSRC)/submesh3D.h 
	$(CC) $(CFLAGS) -o obj/submesh3D.o -c $(GRAPHICSOBJECTSRC)/submesh3D.cpp
	

	
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
	
obj/sceneobjecttransform.o: $(GEOMETRYSRC)/sceneobjecttransform.cpp $(GEOMETRYSRC)/sceneobjecttransform.h 
	$(CC) $(CFLAGS) -o obj/sceneobjecttransform.o -c $(GEOMETRYSRC)/sceneobjecttransform.cpp
	
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
	
obj/longmask.o: $(BASESRC)/longmask.cpp $(BASESRC)/longmask.h 
	$(CC) $(CFLAGS) -o obj/longmask.o -c $(BASESRC)/longmask.cpp
	
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
# Util
# ==================================	

util: $(UTILOBJ)

obj/datastack.o: $(UTILSRC)/datastack.cpp $(UTILSRC)/datastack.h
	$(CC) $(CFLAGS) -o obj/datastack.o -c $(UTILSRC)/datastack.cpp 

obj/util.o: $(UTILSRC)/util.cpp $(UTILSRC)/util.h
	$(CC) $(CFLAGS) -o obj/util.o -c $(UTILSRC)/util.cpp 
	
obj/time.o: $(UTILSRC)/time.cpp $(UTILSRC)/time.h
	$(CC) $(CFLAGS) -o obj/time.o -c $(UTILSRC)/time.cpp 

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
