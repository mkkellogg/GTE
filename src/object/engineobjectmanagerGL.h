#ifndef _ENGINEOBJECTMANAGER_GL_H_
#define _ENGINEOBJECTMANAGER_GL_H_

//forward declaration
class EngineObjectManagerGL;
class Shader;
class Mesh3DRenderer;

#include "engineobjectmanager.h"
#include <vector>

class EngineObjectManagerGL : public EngineObjectManager
{
	friend EngineObjectManager;

    protected:

	EngineObjectManagerGL();
    virtual ~EngineObjectManagerGL();

    public :

    Shader * CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath);
    void DestroyShader(Shader * shader);

    Mesh3DRenderer * CreateMeshRenderer();
    void DestroyMeshRenderer(Mesh3DRenderer * buffer);
};

#endif
