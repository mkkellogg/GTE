#ifndef _ENGINEOBJECTMANAGER_H_
#define _ENGINEOBJECTMANAGER_H_

// forward declarations
class EngineObject;
class SceneObject;
class Shader;
class Mesh3D;
class Mesh3DRenderer;
class EngineObjectManager;

#include <vector>

class EngineObjectManager
{
	static EngineObjectManager * theInstance;

    protected:

	std::vector<EngineObject *> engineObjects;
	std::vector<SceneObject *> sceneObjects;

	EngineObjectManager();
    virtual ~EngineObjectManager();

    public :

    static EngineObjectManager * Instance();
    SceneObject * CreateSceneObject();

    Mesh3D * CreateMesh3D();
    Mesh3DRenderer * CreateMesh3DRenderer();
    Shader * CreateShader(const char * vertexSourcePath, const char * fragmentSourcePath);
};

#endif
