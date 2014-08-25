#ifndef _ENGINEOBJECTMANAGER_H_
#define _ENGINEOBJECTMANAGER_H_

// forward declarations
class EngineObject;
class SceneObject;
class Shader;
class Mesh3D;
class Mesh3DRenderer;
class EngineObjectManager;
class Material;
class Camera;

#include <vector>
#include "graphics/attributes.h"
#include "object/sceneobject.h"

class EngineObjectManager
{
	static EngineObjectManager * theInstance;

    protected:

	std::vector<EngineObject *> engineObjects;
	SceneObject sceneRoot;

	EngineObjectManager();
    virtual ~EngineObjectManager();

    public :

    static EngineObjectManager * Instance();
    SceneObject * CreateSceneObject();

    Mesh3D * CreateMesh3D(AttributeSet attributes);
    Mesh3DRenderer * CreateMesh3DRenderer();
    Shader * CreateShader(const char * vertexSourcePath, const char * fragmentSourcePath);
    Material * CreateMaterial(Shader * shader);
    Material * CreateMaterial(const char * shaderVertexSourcePath, const char * shaderFragmentSourcePath);
    Camera * CreateCamera();

    const SceneObject * GetSceneRoot() const;
};

#endif