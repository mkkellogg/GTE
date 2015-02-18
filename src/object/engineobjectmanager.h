#ifndef _GTE_ENGINEOBJECTMANAGER_H_
#define _GTE_ENGINEOBJECTMANAGER_H_

// forward declarations
class EngineObject;
class SceneObject;
class Shader;
class SubMesh3D;
class Mesh3D;
class Mesh3DFilter;
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
class Skeleton;
class Animation;
class AnimationInstance;
class RenderTarget;
class ShaderSource;

#include <vector>
#include <memory>
#include <unordered_map>
#include "graphics/stdattributes.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "graphics/texture/textureattr.h"
#include "shaderorganizer.h"
#include "base/longmask.h"
#include "base/intmask.h"

class EngineObjectManager
{
	friend class Engine;

	std::unordered_map<ObjectID, SceneObjectRef> sceneObjectDirectory;

	ShaderOrganizer loadedShaders;
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
	void DeleteMesh3DFilter(Mesh3DFilter * filter);
	void DeleteMesh3DRenderer(Mesh3DRenderer * renderer);
	void DeleteSkinnedMesh3DRenderer(SkinnedMesh3DRenderer * renderer);
	void DeleteSubMesh3D(SubMesh3D * mesh);
	void DeleteSubMesh3DRenderer(SubMesh3DRenderer * renderer);
	void DeleteMaterial(Material * material);
	void DeleteTexture(Texture * texture);
	void DeleteShader(Shader * shader);
	void DeleteSkeleton(Skeleton * target);
	void DeleteAnimation(Animation * animation);
	void DeleteAnimationInstance(AnimationInstance * animation);
	void DeleteAnimationPlayer(AnimationPlayer * player);
	void DeleteRenderTarget(RenderTarget * target);

	bool InitBuiltinShaders();

    public :

	bool Init();

    ShaderRef GetLoadedShader(LongMask properties);

    SceneObjectRef FindSceneObjectInDirectory(unsigned long objectID);
    const SceneObjectRef GetSceneRoot() const;
    SceneObjectRef CreateSceneObject();
    void DestroySceneObject(SceneObjectRef sceneObject);

    Mesh3DRef CreateMesh3D(unsigned int subMeshCount);
    void DestroyMesh3D(Mesh3DRef mesh);
    Mesh3DFilterRef CreateMesh3DFilter();
    void DestroyMesh3DFilter(Mesh3DFilterRef filter);
    Mesh3DRendererRef CreateMesh3DRenderer();
    void DestroyMesh3DRenderer(Mesh3DRendererRef renderer);
    SkinnedMesh3DRendererRef CreateSkinnedMesh3DRenderer();
    void DestroySkinnedMesh3DRenderer(SkinnedMesh3DRendererRef renderer);
    SubMesh3DRef CreateSubMesh3D(StandardAttributeSet attributes);
    void DestroySubMesh3D(SubMesh3DRef mesh);
    SubMesh3DRendererRef CreateSubMesh3DRenderer(AttributeTransformer * attrTransformer);
    SubMesh3DRendererRef CreateSubMesh3DRenderer();
    void DestroySubMesh3DRenderer(SubMesh3DRendererRef renderer);

    SkeletonRef CreateSkeleton(unsigned int boneCount);
    SkeletonRef CloneSkeleton(SkeletonRef source);
    void DestroySkeleton(SkeletonRef skeleton);

    AnimationRef CreateAnimation(float duration, float ticksPerSecond);
    void DestroyAnimation(AnimationRef animation);
    AnimationInstanceRef CreateAnimationInstance(SkeletonRef target, AnimationRef animation);
    void DestroyAnimationInstance(AnimationInstanceRef instance);
    AnimationPlayerRef CreateAnimationPlayer(SkeletonRef target);
    void DestroyAnimationPlayer(AnimationPlayerRef player);

    ShaderRef CreateShader(const ShaderSource& shaderSource);
    void DestroyShader(ShaderRef shader);
    TextureRef CreateTexture(const char * sourcePath, TextureAttributes attributes);
    TextureRef CreateTexture(const RawImage * imageData, const char * sourcePath, TextureAttributes attributes);
    TextureRef CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
   		    						    const std::string& bottom, const std::string& left, const std::string& right);
    TextureRef CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
   										RawImage * bottomData, RawImage * leftData, RawImage * rightData,
   										const std::string& front, const std::string& back, const std::string& top,
   										const std::string& bottom, const std::string& left, const std::string& right);
    void DestroyTexture(TextureRef texture);
    MaterialRef CreateMaterial(const char *name, ShaderRef shader);
    MaterialRef CreateMaterial(const char *name, const ShaderSource& shaderSource);
    MaterialRef CreateMaterial(const std::string& name, const ShaderSource& shaderSource);

    void DestroyMaterial(MaterialRef material);
    RenderTargetRef CreateRenderTarget(IntMask buffers, unsigned int width, unsigned int height);
    void DestroyRenderTarget(RenderTargetRef buffer);

    CameraRef CreateCamera();
    void DestroyCamera(CameraRef camera);
    LightRef CreateLight();
    void DestroyLight(LightRef light);
};

#endif
