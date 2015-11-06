#ifndef _GTE_ENGINEOBJECTMANAGER_H_
#define _GTE_ENGINEOBJECTMANAGER_H_

#include <vector>
#include <memory>
#include <unordered_map>
#include "graphics/stdattributes.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "object/layermanager.h"
#include "graphics/texture/textureattr.h"
#include "shaderorganizer.h"
#include "base/longmask.h"
#include "base/intmask.h"

namespace GTE
{
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
	class Atlas;
	class Light;
	class RawImage;
	class AttributeTransformer;
	class Skeleton;
	class Animation;
	class AnimationInstance;
	class RenderTarget;
	class ShaderSource;

	class EngineObjectManager
	{
		friend class Engine;
		friend class Graphics;

		std::unordered_map<ObjectID, SceneObjectRef> sceneObjectDirectory;

		LayerManager layerManager;
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
		void DeleteAtlas(Atlas * atlas);
		void DeleteRenderTarget(RenderTarget * target);
		void DeleteShader(Shader * shader);
		void DeleteSkeleton(Skeleton * target);
		void DeleteAnimation(Animation * animation);
		void DeleteAnimationInstance(AnimationInstance * animation);
		void DeleteAnimationPlayer(AnimationPlayer * player);

		Bool InitBuiltinShaders();

		RenderTargetRef WrapRenderTarget(RenderTarget * target);

	public:

		static const std::string DefaultLayer;

		Bool Init();

		ShaderRef GetLoadedShader(LongMask properties);

		LayerManager& GetLayerManager();

		SceneObjectRef FindSceneObjectInDirectory(unsigned long objectID);
		const SceneObjectRef& GetSceneRoot() const;
		SceneObjectRef CreateSceneObject();
		void DestroySceneObject(SceneObjectRef sceneObject);

		Mesh3DRef CreateMesh3D(UInt32 subMeshCount);
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

		SkeletonRef CreateSkeleton(UInt32 boneCount);
		SkeletonRef CloneSkeleton(SkeletonRef source);
		void DestroySkeleton(SkeletonRef skeleton);

		AnimationRef CreateAnimation(Real duration, Real ticksPerSecond);
		void DestroyAnimation(AnimationRef animation);
		AnimationInstanceRef CreateAnimationInstance(SkeletonRef target, AnimationRefConst animation);
		void DestroyAnimationInstance(AnimationInstanceRef instance);
		AnimationPlayerRef CreateAnimationPlayer(SkeletonRef target);
		void DestroyAnimationPlayer(AnimationPlayerRef player);

		ShaderRef CreateShader(const ShaderSource& shaderSource);
		void DestroyShader(ShaderRef shader);
		TextureRef CreateTexture(const std::string& sourcePath, TextureAttributes attributes);
		TextureRef CreateTexture(RawImage * imageData, TextureAttributes attributes);
		TextureRef CreateTexture(UInt32 width, UInt32 height, Byte * pixelData, TextureAttributes attributes);
		TextureRef CreateCubeTexture(Byte * frontData, UInt32 fw, UInt32 fh,
			Byte * backData, UInt32 backw, UInt32 backh,
			Byte * topData, UInt32 tw, UInt32 th,
			Byte * bottomData, UInt32 botw, UInt32 both,
			Byte * leftData, UInt32 lw, UInt32 lh,
			Byte * rightData, UInt32 rw, UInt32 rh);
		TextureRef CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
			const std::string& bottom, const std::string& left, const std::string& right);
		TextureRef CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
			RawImage * bottomData, RawImage * leftData, RawImage * rightData);
		void DestroyTexture(TextureRef texture);
		AtlasRef CreateAtlas(TextureRef texture, Bool createFirstFullImage);
		void DestroyAtlas(AtlasRef atlas);
		RenderTargetRef CreateRenderTarget(Bool hasColor, Bool hasDepth, Bool enableStencilBuffer,
			const TextureAttributes& colorTextureAttributes,
			UInt32 width, UInt32 height);
		void DestroyRenderTarget(RenderTargetRef target);

		MaterialRef CreateMaterial(const std::string& name, ShaderRef shader);
		MaterialRef CreateMaterial(const std::string& name, const ShaderSource& shaderSource);
		void DestroyMaterial(MaterialRef material);

		CameraRef CreateCamera();
		void DestroyCamera(CameraRef camera);
		LightRef CreateLight();
		void DestroyLight(LightRef light);
	};
}

#endif
