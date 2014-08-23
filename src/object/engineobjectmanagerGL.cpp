#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engineobjectmanagerGL.h"
#include "graphics/shader/shaderGL.h"
#include "graphics/render/mesh3DrendererGL.h"

EngineObjectManagerGL::EngineObjectManagerGL()
{

}

EngineObjectManagerGL::~EngineObjectManagerGL()
{

}

Shader * EngineObjectManagerGL::CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath)
{
    //TODO: Add switch for different platforms; for now only support OpenGL
    Shader * shader = new ShaderGL(vertexShaderPath, fragmentShaderPath);
    return shader;
}

void EngineObjectManagerGL::DestroyShader(Shader * shader)
{
    delete shader;
}

Mesh3DRenderer * EngineObjectManagerGL::CreateMeshRenderer()
{
	 //TODO: Add switch for different platforms; for now only support OpenGL
	return new Mesh3DRendererGL();
}

void EngineObjectManagerGL::DestroyMeshRenderer(Mesh3DRenderer * renderer)
{
	delete renderer;
}
