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
class TextureAttributes;
class Texture;

#include <vector>
#include "graphics/attributes.h"
#include "object/sceneobject.h"
#include "graphics/texture/textureattr.h"

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

    Mesh3D * CreateMesh3D(StandardAttributeSet attributes);
    void DestroyMesh3D(Mesh3D * mesh);
    Mesh3DRenderer * CreateMesh3DRenderer();
    void DestroyMesh3DRenderer(Mesh3DRenderer * renderer);
    Shader * CreateShader(const char * vertexSourcePath, const char * fragmentSourcePath);
    void DestroyShader(Shader * shader);
    Texture * CreateTexture(const char * sourcePath, TextureAttributes attributes);
    Material * CreateMaterial(const char *name,Shader * shader);
    Material * CreateMaterial(const char *name, const char * shaderVertexSourcePath, const char * shaderFragmentSourcePath);
    void DestroyMaterial(Material * material);
    Camera * CreateCamera();
    void DestroyCamera(Camera * camera);

    const SceneObject * GetSceneRoot() const;
};

#endif
