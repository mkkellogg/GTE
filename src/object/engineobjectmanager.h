#ifndef _ENGINEOBJECTMANAGER_H_
#define _ENGINEOBJECTMANAGER_H_

// forward declarations
class EngineObject;
class SceneObject;
class Shader;
class Mesh3DRenderer;

#include <vector>

class EngineObjectManager
{
    protected:

	std::vector<EngineObject *> engineObjects;
	std::vector<SceneObject *> sceneObjects;

	EngineObjectManager();
    virtual ~EngineObjectManager();

    public :

    virtual Shader * CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath) = 0;
    virtual void DestroyShader(Shader * shader) = 0;

    virtual Mesh3DRenderer * CreateMeshRenderer()  = 0;
    virtual void DestroyMeshRenderer(Mesh3DRenderer * buffer) = 0;

};

#endif
