#ifndef _ENGINEOBJECTMANAGER_H_
#define _ENGINEOBJECTMANAGER_H_

// forward declarations
class EngineObject;
class SceneObject;
class Shader;
class SubMesh3D;
class Mesh3D;
class SubMesh3DRenderer;
class Mesh3DRenderer;
class EngineObjectManager;
class Material;
class Camera;
class TextureAttributes;
class Texture;
class Light;
class RawImage;

#include <vector>
#include "graphics/stdattributes.h"
#include "object/sceneobject.h"
#include "graphics/texture/textureattr.h"
#include "shadermanager.h"
#include "base/longmask.h"


class EngineObjectManager
{
	static EngineObjectManager * theInstance;

	const char* builtinPath ="resources/builtin/";

	ShaderManager loadedShaders;
	std::vector<EngineObject *> engineObjects;
	SceneObject sceneRoot;
	unsigned long currentEngineObjectID;

	unsigned long GetNextObjectID();
	EngineObjectManager();
	virtual ~EngineObjectManager();

    public :

    static EngineObjectManager * Instance();
    SceneObject * CreateSceneObject();
    bool InitBuiltinShaders();
    Shader * GetLoadedShader(LongMask properties);

    Mesh3D * CreateMesh3D(unsigned int subMeshCount);
    void DestroyMesh3D(Mesh3D * mesh);
    Mesh3DRenderer * CreateMesh3DRenderer();
    void DestroyMesh3DRenderer(Mesh3DRenderer * renderer);

    SubMesh3D * CreateSubMesh3D(StandardAttributeSet attributes);
    void DestroySubMesh3D(SubMesh3D * mesh);
    SubMesh3DRenderer * CreateSubMesh3DRenderer();
    void DestroySubMesh3DRenderer(SubMesh3DRenderer * renderer);
    Shader * CreateShader(const char * vertexSourcePath, const char * fragmentSourcePath);
    void DestroyShader(Shader * shader);
    Texture * CreateTexture(const char * sourcePath, TextureAttributes attributes);
    Texture * CreateTexture(const RawImage * imageData, const char * sourcePath, TextureAttributes attributes);
    Material * CreateMaterial(const char *name,Shader * shader);
    Material * CreateMaterial(const char *name, const char * shaderVertexSourcePath, const char * shaderFragmentSourcePath);
    void DestroyMaterial(Material * material);
    Camera * CreateCamera();
    void DestroyCamera(Camera * camera);
    Light * CreateLight();
    void DestroyLight(Light * light);

    const SceneObject * GetSceneRoot() const;
};

#endif
