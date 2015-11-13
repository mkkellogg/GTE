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
	class ParticleSystem;

	class EngineObjectManager
	{
		friend class Engine;
		friend class Graphics;

		std::unordered_map<ObjectID, SceneObjectSharedPtr> sceneObjectDirectory;

		LayerManager layerManager;
		ShaderOrganizer loadedShaders;
		std::vector<EngineObject *> engineObjects;
		SceneObject sceneRoot;
		SceneObjectSharedPtr sceneRootRef;
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
		void DeleteParticleSystem(ParticleSystem * system);

		Bool InitBuiltinShaders();

		RenderTargetSharedPtr WrapRenderTarget(RenderTarget * target);

	public:

		static const std::string DefaultLayer;

		Bool Init();

		ShaderSharedPtr GetLoadedShader(LongMask properties);

		LayerManager& GetLayerManager();

		SceneObjectRef FindSceneObjectInDirectory(unsigned long objectID);
		SceneObjectRef GetSceneRoot() const;
		SceneObjectSharedPtr CreateSceneObject();
		void DestroySceneObject(SceneObjectSharedPtr sceneObject);

		Mesh3DSharedPtr CreateMesh3D(UInt32 subMeshCount);
		void DestroyMesh3D(Mesh3DSharedPtr mesh);
		Mesh3DFilterSharedPtr CreateMesh3DFilter();
		void DestroyMesh3DFilter(Mesh3DFilterSharedPtr filter);
		Mesh3DRendererSharedPtr CreateMesh3DRenderer();
		void DestroyMesh3DRenderer(Mesh3DRendererSharedPtr renderer);
		SkinnedMesh3DRendererSharedPtr CreateSkinnedMesh3DRenderer();
		void DestroySkinnedMesh3DRenderer(SkinnedMesh3DRendererSharedPtr renderer);
		SubMesh3DSharedPtr CreateSubMesh3D(StandardAttributeSet attributes);
		void DestroySubMesh3D(SubMesh3DSharedPtr mesh);
		SubMesh3DRendererSharedPtr CreateSubMesh3DRenderer(AttributeTransformer * attrTransformer);
		SubMesh3DRendererSharedPtr CreateSubMesh3DRenderer();
		void DestroySubMesh3DRenderer(SubMesh3DRendererSharedPtr renderer);

		SkeletonSharedPtr CreateSkeleton(UInt32 boneCount);
		SkeletonSharedPtr CloneSkeleton(SkeletonSharedPtr source);
		void DestroySkeleton(SkeletonSharedPtr skeleton);

		AnimationSharedPtr CreateAnimation(Real duration, Real ticksPerSecond);
		void DestroyAnimation(AnimationSharedPtr animation);
		AnimationInstanceSharedPtr CreateAnimationInstance(SkeletonSharedPtr target, AnimationSharedConstPtr animation);
		void DestroyAnimationInstance(AnimationInstanceSharedPtr instance);
		AnimationPlayerSharedPtr CreateAnimationPlayer(SkeletonSharedPtr target);
		void DestroyAnimationPlayer(AnimationPlayerSharedPtr player);

		ShaderSharedPtr CreateShader(const ShaderSource& shaderSource);
		void DestroyShader(ShaderSharedPtr shader);
		TextureSharedPtr CreateTexture(const std::string& sourcePath, TextureAttributes attributes);
		TextureSharedPtr CreateTexture(RawImage * imageData, TextureAttributes attributes);
		TextureSharedPtr CreateTexture(UInt32 width, UInt32 height, Byte * pixelData, TextureAttributes attributes);
		TextureSharedPtr CreateCubeTexture(Byte * frontData, UInt32 fw, UInt32 fh,
			Byte * backData, UInt32 backw, UInt32 backh,
			Byte * topData, UInt32 tw, UInt32 th,
			Byte * bottomData, UInt32 botw, UInt32 both,
			Byte * leftData, UInt32 lw, UInt32 lh,
			Byte * rightData, UInt32 rw, UInt32 rh);
		TextureSharedPtr CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
			const std::string& bottom, const std::string& left, const std::string& right);
		TextureSharedPtr CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
			RawImage * bottomData, RawImage * leftData, RawImage * rightData);
		void DestroyTexture(TextureSharedPtr texture);
		AtlasSharedPtr CreateAtlas(TextureSharedPtr texture, Bool createFirstFullImage);
		AtlasSharedPtr CreateGridAtlas(TextureSharedPtr texture, Real left, Real top, Real right, Real bottom, UInt32 xCount, UInt32 yCount, Bool reverseX, Bool reverseY);
		void DestroyAtlas(AtlasSharedPtr atlas);
		RenderTargetSharedPtr CreateRenderTarget(Bool hasColor, Bool hasDepth, Bool enableStencilBuffer,
			const TextureAttributes& colorTextureAttributes,
			UInt32 width, UInt32 height);
		void DestroyRenderTarget(RenderTargetSharedPtr target);

		MaterialSharedPtr CreateMaterial(const std::string& name, ShaderSharedPtr shader);
		MaterialSharedPtr CreateMaterial(const std::string& name, const ShaderSource& shaderSource);
		void DestroyMaterial(MaterialSharedPtr material);

		ParticleSystemSharedPtr CreateParticleSystem(MaterialRef material, AtlasRef atlas, Bool zSort, Real releaseRate, Real particleLifeSpan, Real systemLifeSpan);
		void DestroyParticleSystem(ParticleSystemSharedPtr system);

		CameraSharedPtr CreateCamera();
		void DestroyCamera(CameraSharedPtr camera);
		LightSharedPtr CreateLight();
		void DestroyLight(LightSharedPtr light);
	};
}

#endif
