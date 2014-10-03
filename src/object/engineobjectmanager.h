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
class AttributeTransformer;

#include <vector>
#include <memory>
#include <unordered_map>
#include "graphics/stdattributes.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "graphics/texture/textureattr.h"
#include "shadermanager.h"
#include "base/longmask.h"


class EngineObjectManager
{
	static EngineObjectManager * theInstance;

	const char* builtinPath ="resources/builtin/";

	std::unordered_map<unsigned long, SceneObjectRef> sceneObjectDirectory;

	ShaderManager loadedShaders;
	std::vector<EngineObject *> engineObjects;
	SceneObject sceneRoot;
	SceneObjectRef sceneRootRef;
	unsigned long currentEngineObjectID;

	unsigned long GetNextObjectID();
	EngineObjectManager();
	virtual ~EngineObjectManager();

	void AddSceneObjectToDirectory(unsigned long objectID, SceneObjectRef ref);

	void DeleteSceneObject(SceneObject * sceneObject);
	void DeleteLight(Light * light);
	void DeleteCamera(Camera * light);
	void DeleteMesh3D(Mesh3D * mesh);
	void DeleteMesh3DRenderer(Mesh3DRenderer * renderer);
	void DeleteSubMesh3D(SubMesh3D * mesh);
	void DeleteSubMesh3DRenderer(SubMesh3DRenderer * renderer);
	void DeleteMaterial(Material * material);

    public :

    static EngineObjectManager * Instance();
    bool InitBuiltinShaders();
    Shader * GetLoadedShader(LongMask properties);

    SceneObjectRef FindSceneObjectInDirectory(unsigned long objectID);
    const SceneObjectRef GetSceneRoot() const;
    SceneObjectRef CreateSceneObject();
    void DestroySceneObject(SceneObjectRef sceneObject);

    Mesh3DRef CreateMesh3D(unsigned int subMeshCount);
    void DestroyMesh3D(Mesh3DRef mesh);
    Mesh3DRendererRef CreateMesh3DRenderer();
    void DestroyMesh3DRenderer(Mesh3DRendererRef renderer);

    SubMesh3DRef CreateSubMesh3D(StandardAttributeSet attributes);
    void DestroySubMesh3D(SubMesh3DRef mesh);
    SubMesh3DRendererRef CreateSubMesh3DRenderer(AttributeTransformer * attrTransformer);
    SubMesh3DRendererRef CreateSubMesh3DRenderer();
    void DestroySubMesh3DRenderer(SubMesh3DRendererRef renderer);
    Shader * CreateShader(const char * vertexSourcePath, const char * fragmentSourcePath);
    void DestroyShader(Shader * shader);
    Texture * CreateTexture(const char * sourcePath, TextureAttributes attributes);
    Texture * CreateTexture(const RawImage * imageData, const char * sourcePath, TextureAttributes attributes);
    MaterialRef CreateMaterial(const char *name,Shader * shader);
    MaterialRef CreateMaterial(const char *name, const char * shaderVertexSourcePath, const char * shaderFragmentSourcePath);
    void DestroyMaterial(MaterialRef material);
    CameraRef CreateCamera();
    void DestroyCamera(CameraRef camera);
    LightRef CreateLight();
    void DestroyLight(LightRef light);
};

#endif
