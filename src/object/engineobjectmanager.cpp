#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engine.h"
#include "engineobjectmanager.h"
#include "sceneobject.h"
#include "enginetypes.h"
#include "graphics/graphics.h"
#include "graphics/shader/shader.h"
#include "graphics/light/light.h"
#include "graphics/render/material.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/attributetransformer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/stdattributes.h"
#include "graphics/texture/texture.h"
#include "graphics/image/rawimage.h"
#include "graphics/animation/skeleton.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/animationinstance.h"
#include "graphics/animation/animationplayer.h"
#include "asset/assetimporter.h"
#include "debug/gtedebug.h"
#include "graphics/view/camera.h"
#include "base/longmask.h"
#include "base/intmask.h"
#include "util/engineutility.h"
#include "filesys/filesystem.h"
#include "global/constants.h"
#include <string>

namespace GTE
{
	const std::string EngineObjectManager::DefaultLayer = "Default";

	EngineObjectManager::EngineObjectManager()
	{
		currentEngineObjectID = 0L;

		sceneRoot.SetObjectID(GetNextObjectID());
		sceneRootRef = SceneObjectRef(&sceneRoot, [=](SceneObject * sceneObject)
		{

		});
		AddSceneObjectToDirectory(sceneRoot.GetObjectID(), sceneRootRef);
	}

	EngineObjectManager::~EngineObjectManager()
	{

	}

	unsigned long EngineObjectManager::GetNextObjectID()
	{
		return currentEngineObjectID++;
	}

	bool EngineObjectManager::Init()
	{
		layerManager.AddLayer(DefaultLayer);
		return true;
	}

	bool EngineObjectManager::InitBuiltinShaders()
	{
		std::string vertexSource;
		std::string fragmentSource;
		ShaderSource shaderSource;
		ShaderRef shader;
		AssetImporter assetImporter;

		FileSystem * fileSystem = FileSystem::Instance();
		std::string builtinPath = fileSystem->FixupPathForLocalFilesystem(Constants::BuiltinShaderPath);

		LongMask shaderProperties;

		assetImporter.LoadBuiltInShaderSource("diffuse", shaderSource);
		shader = CreateShader(shaderSource);
		ASSERT(shader.IsValid(), "EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseColored");
		shaderProperties = LongMaskUtil::CreateLongMask();
		LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::DiffuseColored);
		LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexNormals);
		loadedShaders.AddShader(shaderProperties, shader);

		assetImporter.LoadBuiltInShaderSource("diffuse_texture", shaderSource);
		shader = CreateShader(shaderSource);
		ASSERT(shader.IsValid(), "EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseTextured");
		shaderProperties = LongMaskUtil::CreateLongMask();
		LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::DiffuseTextured);
		LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexNormals);
		loadedShaders.AddShader(shaderProperties, shader);

		assetImporter.LoadBuiltInShaderSource("diffuse_texture_vcolor", shaderSource);
		shader = CreateShader(shaderSource);
		ASSERT(shader.IsValid(), "EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseTextured & VertexColors");
		shaderProperties = LongMaskUtil::CreateLongMask();
		LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::DiffuseTextured);
		LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexColors);
		LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexNormals);
		loadedShaders.AddShader(shaderProperties, shader);

		return true;
	}

	SceneObjectRef EngineObjectManager::FindSceneObjectInDirectory(unsigned long objectID)
	{
		if (sceneObjectDirectory.find(objectID) != sceneObjectDirectory.end())
		{
			SceneObjectRef ref = sceneObjectDirectory[objectID];
			return ref;
		}

		return SceneObjectRef::Null();
	}

	void EngineObjectManager::AddSceneObjectToDirectory(unsigned long objectID, SceneObjectRef ref)
	{
		ASSERT(ref.IsValid(), "EngineObjectManager::AddSceneObjectToDirectory -> Tried to add invalid scene object reference.");
		sceneObjectDirectory[objectID] = ref;
	}

	ShaderRef EngineObjectManager::GetLoadedShader(LongMask properties)
	{
		return loadedShaders.GetShader(properties);
	}

	LayerManager& EngineObjectManager::GetLayerManager()
	{
		return layerManager;
	}

	SceneObjectRef EngineObjectManager::CreateSceneObject()
	{
		SceneObject *sceneObject = new SceneObject();
		ASSERT(sceneObject != NULL, "EngineObjectManager::CreateSceneObject -> could not allocate new scene object.");
		sceneObject->SetObjectID(GetNextObjectID());

		SceneObjectRef ref(sceneObject, [=](SceneObject * sceneObject)
		{
			DeleteSceneObject(sceneObject);
		});

		sceneObject->SetActive(true);

		ref->SetLayerMask(layerManager.GetLayerMask(DefaultLayer));
		AddSceneObjectToDirectory(ref->GetObjectID(), ref);

		sceneRootRef->AddChild(ref);
		return ref;
	}

	void EngineObjectManager::DestroySceneObject(SceneObjectRef sceneObject)
	{
		NONFATAL_ASSERT(sceneObject.IsValid(), "EngineObjectManager::DestroySceneObject -> 'sceneObject' is invalid.", true);
		sceneObject.ForceDelete();
	}

	void EngineObjectManager::DeleteSceneObject(SceneObject * sceneObject)
	{
		ASSERT(sceneObject != NULL, "EngineObjectManager::DeleteSceneObject -> sceneObject is NULL.");
		delete sceneObject;
	}

	const SceneObjectRef EngineObjectManager::GetSceneRoot() const
	{
		return (const SceneObjectRef)sceneRootRef;
	}

	Mesh3DRef EngineObjectManager::CreateMesh3D(unsigned int subMeshCount)
	{
		Mesh3D * mesh = new Mesh3D(subMeshCount);
		ASSERT(mesh != NULL, "EngineObjectManager::CreateMesh3D -> Could not allocate new Mesh3D object.");
		mesh->SetObjectID(GetNextObjectID());

		return Mesh3DRef(mesh, [=](Mesh3D * mesh)
		{
			DeleteMesh3D(mesh);
		});
	}

	void EngineObjectManager::DestroyMesh3D(Mesh3DRef mesh)
	{
		mesh.ForceDelete();
	}

	void EngineObjectManager::DeleteMesh3D(Mesh3D * mesh)
	{
		ASSERT(mesh != NULL, "EngineObjectManager::DeleteMesh -> 'mesh' is null.");
		delete mesh;
	}

	Mesh3DFilterRef EngineObjectManager::CreateMesh3DFilter()
	{
		Mesh3DFilter * filter = new Mesh3DFilter();
		ASSERT(filter != NULL, "EngineObjectManager::CreateMesh3DFilter -> Could not allocate new Mesh3DFilter object.");
		filter->SetObjectID(GetNextObjectID());

		return Mesh3DFilterRef(filter, [=](Mesh3DFilter * filter)
		{
			DeleteMesh3DFilter(filter);
		});
	}

	void EngineObjectManager::DestroyMesh3DFilter(Mesh3DFilterRef filter)
	{
		filter.ForceDelete();
	}

	void EngineObjectManager::DeleteMesh3DFilter(Mesh3DFilter * filter)
	{
		ASSERT(filter != NULL, "EngineObjectManager::DeleteMesh3DFilter -> 'filter' is NULL.");
		delete filter;
	}

	Mesh3DRendererRef EngineObjectManager::CreateMesh3DRenderer()
	{
		Mesh3DRenderer * renderer = new Mesh3DRenderer();
		ASSERT(renderer != NULL, "EngineObjectManager::CreateMesh3DRenderer -> Could not allocate new Mesh3DRenderer object.");
		renderer->SetObjectID(GetNextObjectID());

		return Mesh3DRendererRef(renderer, [=](Mesh3DRenderer * renderer)
		{
			DeleteMesh3DRenderer(renderer);
		});
	}

	void EngineObjectManager::DestroyMesh3DRenderer(Mesh3DRendererRef renderer)
	{
		renderer.ForceDelete();
	}

	void EngineObjectManager::DeleteMesh3DRenderer(Mesh3DRenderer * renderer)
	{
		ASSERT(renderer != NULL, "EngineObjectManager::DeleteMesh3DRenderer -> 'renderer' is null.");
		delete renderer;
	}

	SkinnedMesh3DRendererRef EngineObjectManager::CreateSkinnedMesh3DRenderer()
	{
		SkinnedMesh3DRenderer * renderer = new SkinnedMesh3DRenderer();
		ASSERT(renderer != NULL, "EngineObjectManager::CreateSkinnedMesh3DRenderer -> Could not allocate new SkinnedMesh3DRenderer object.");
		renderer->SetObjectID(GetNextObjectID());

		return SkinnedMesh3DRendererRef(renderer, [=](SkinnedMesh3DRenderer * renderer)
		{
			DeleteSkinnedMesh3DRenderer(renderer);
		});
	}

	void EngineObjectManager::DestroySkinnedMesh3DRenderer(SkinnedMesh3DRendererRef renderer)
	{
		renderer.ForceDelete();
	}

	void EngineObjectManager::DeleteSkinnedMesh3DRenderer(SkinnedMesh3DRenderer * renderer)
	{
		ASSERT(renderer != NULL, "EngineObjectManager::DeleteSkinnedMesh3DRenderer -> 'renderer' is null.");
		delete renderer;
	}

	SubMesh3DRef EngineObjectManager::CreateSubMesh3D(StandardAttributeSet attributes)
	{
		SubMesh3D * mesh = new SubMesh3D(attributes);
		ASSERT(mesh != NULL, "EngineObjectManager::CreateSubMesh3D -> could not allocate new SubMesh3D object.");
		mesh->SetObjectID(GetNextObjectID());

		return SubMesh3DRef(mesh, [=](SubMesh3D * mesh)
		{
			DeleteSubMesh3D(mesh);
		});
	}

	void EngineObjectManager::DestroySubMesh3D(SubMesh3DRef mesh)
	{
		NONFATAL_ASSERT(mesh.IsValid(), "EngineObjectManager::DestroySubMesh3D -> 'mesh' is invalid.", true);
		mesh.ForceDelete();
	}

	void EngineObjectManager::DeleteSubMesh3D(SubMesh3D * mesh)
	{
		ASSERT(mesh != NULL, "EngineObjectManager::DeleteSubMesh3D -> 'mesh' is null.");
		delete mesh;
	}

	SubMesh3DRendererRef  EngineObjectManager::CreateSubMesh3DRenderer()
	{
		return CreateSubMesh3DRenderer(NULL);
	}

	SubMesh3DRendererRef EngineObjectManager::CreateSubMesh3DRenderer(AttributeTransformer * attrTransformer)
	{
		// TODO: for now we force vertex attribute buffers to be on GPU
		// in the constructor for SubMesh3DRenderer, need to think
		// of a better way to do this.
		SubMesh3DRenderer * renderer = new SubMesh3DRenderer(true, attrTransformer);

		ASSERT(renderer != NULL, "EngineObjectManager::CreateMesh3DRenderer(AttributeTransformer) -> could not allocate new SubMesh3DRenderer object.");
		renderer->SetObjectID(GetNextObjectID());

		return SubMesh3DRendererRef(renderer, [=](SubMesh3DRenderer * renderer)
		{
			DeleteSubMesh3DRenderer(renderer);
		});
	}

	void EngineObjectManager::DestroySubMesh3DRenderer(SubMesh3DRendererRef renderer)
	{
		NONFATAL_ASSERT(renderer.IsValid(), "EngineObjectManager::DestroySubMesh3DRenderer -> 'renderer' is invalid.", true);
		renderer.ForceDelete();
	}

	void EngineObjectManager::DeleteSubMesh3DRenderer(SubMesh3DRenderer * renderer)
	{
		ASSERT(renderer != NULL, "EngineObjectManager::DeleteSubMesh3DRenderer -> 'renderer' is null.");
		delete renderer;
	}

	SkeletonRef EngineObjectManager::CreateSkeleton(unsigned int boneCount)
	{
		Skeleton * skeleton = new Skeleton(boneCount);
		ASSERT(skeleton != NULL, "EngineObjectManager::CreateSkeleton -> Could not allocate new skeleton.");

		return SkeletonRef(skeleton, [=](Skeleton * skeleton)
		{
			DeleteSkeleton(skeleton);
		});
	}

	SkeletonRef EngineObjectManager::CloneSkeleton(SkeletonRef source)
	{
		NONFATAL_ASSERT_RTRN(source.IsValid(), "EngineObjectManager::CloneSkeleton -> 'source' is invalid.", SkeletonRef::Null(), true);

		Skeleton * skeleton = source->FullClone();
		NONFATAL_ASSERT_RTRN(skeleton != NULL, "EngineObjectManager::CloneSkeleton -> Could not clone source.", SkeletonRef::Null(), true);

		return SkeletonRef(skeleton, [=](Skeleton * skeleton)
		{
			DeleteSkeleton(skeleton);
		});
	}

	void EngineObjectManager::DestroySkeleton(SkeletonRef target)
	{
		target.ForceDelete();
	}

	void EngineObjectManager::DeleteSkeleton(Skeleton * target)
	{
		ASSERT(target != NULL, "EngineObjectManager::DeleteSkeleton -> 'skeleton' is null.");
		delete target;
	}

	AnimationRef EngineObjectManager::CreateAnimation(float duration, float ticksPerSecond)
	{
		Animation * animation = new Animation(duration, ticksPerSecond);
		ASSERT(animation != NULL, "EngineObjectManager::CreateAnimation -> Could not allocate new Animation object.");
		animation->SetObjectID(GetNextObjectID());

		return AnimationRef(animation, [=](Animation * animation)
		{
			DeleteAnimation(animation);
		});
	}

	void EngineObjectManager::DestroyAnimation(AnimationRef animation)
	{
		animation.ForceDelete();
	}

	void EngineObjectManager::DeleteAnimation(Animation * animation)
	{
		ASSERT(animation != NULL, "EngineObjectManager::CreateAnimation -> 'animation' is null.");
		delete animation;
	}

	AnimationInstanceRef EngineObjectManager::CreateAnimationInstance(SkeletonRef target, AnimationRef animation)
	{
		NONFATAL_ASSERT_RTRN(target.IsValid(), "EngineObjectManager::CreateAnimationInstance -> 'target' is invalid.", AnimationInstanceRef::Null(), true);
		NONFATAL_ASSERT_RTRN(animation.IsValid(), "EngineObjectManager::CreateAnimationInstance -> 'animation' is invalid.", AnimationInstanceRef::Null(), true);

		AnimationInstance * instance = new AnimationInstance(target, animation);
		ASSERT(instance != NULL, "EngineObjectManager::CreateAnimationInstance -> Could not allocate new AnimationInstance object.");

		return AnimationInstanceRef(instance, [=](AnimationInstance * instance)
		{
			DeleteAnimationInstance(instance);
		});
	}

	void EngineObjectManager::DestroyAnimationInstance(AnimationInstanceRef instance)
	{
		instance.ForceDelete();
	}

	void EngineObjectManager::DeleteAnimationInstance(AnimationInstance * instance)
	{
		ASSERT(instance != NULL, "EngineObjectManager::DeleteAnimationInstance -> 'instance' is null.");
		delete instance;
	}

	AnimationPlayerRef EngineObjectManager::CreateAnimationPlayer(SkeletonRef target)
	{
		NONFATAL_ASSERT_RTRN(target.IsValid(), "EngineObjectManager::CreateAnimationPlayer -> 'target' is invalid.", AnimationPlayerRef::Null(), true);

		AnimationPlayer * player = new AnimationPlayer(target);
		ASSERT(player != NULL, "EngineObjectManager::CreateAnimationPlayer -> Could not allocate new AnimationPlayer object.");

		return AnimationPlayerRef(player, [=](AnimationPlayer * player)
		{
			DeleteAnimationPlayer(player);
		});
	}

	void EngineObjectManager::DestroyAnimationPlayer(AnimationPlayerRef player)
	{
		player.ForceDelete();
	}

	void EngineObjectManager::DeleteAnimationPlayer(AnimationPlayer * player)
	{
		ASSERT(player != NULL, "EngineObjectManager::DeleteAnimationPlayer -> 'player' is null.");
		delete player;
	}

	ShaderRef EngineObjectManager::CreateShader(const ShaderSource& shaderSource)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != NULL, "EngineObjectManager::CreateShader -> Graphics system is null.");

		Shader * shader = graphics->CreateShader(shaderSource);
		NONFATAL_ASSERT_RTRN(shader != NULL, "EngineObjectManager::CreateShader -> Could not create new Shader object.", ShaderRef::Null(), false);
		shader->SetObjectID(GetNextObjectID());

		return ShaderRef(shader, [=](Shader * shader)
		{
			DeleteShader(shader);
		});
	}

	void EngineObjectManager::DestroyShader(ShaderRef shader)
	{
		shader.ForceDelete();
	}

	void EngineObjectManager::DeleteShader(Shader * shader)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != NULL, "EngineObjectManager::DeleteShader -> Graphics system is null.");

		ASSERT(shader != NULL, "EngineObjectManager::DeleteShader -> 'shader' is null.");
		graphics->DestroyShader(shader);
	}

	TextureRef EngineObjectManager::CreateTexture(const std::string& sourcePath, TextureAttributes attributes)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		Texture * texture = graphics->CreateTexture(sourcePath, attributes);

		if (texture == NULL)
		{
			Debug::PrintError("EngineObjectManager::CreateTexture -> could not create new Texture object.");
			return TextureRef::Null();
		}

		texture->SetObjectID(GetNextObjectID());

		return TextureRef(texture, [=](Texture * texture)
		{
			DeleteTexture(texture);
		});
	}

	TextureRef EngineObjectManager::CreateTexture(RawImage * imageData, TextureAttributes attributes)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != NULL, "EngineObjectManager::CreateTexture -> Graphics system is null.");

		Texture * texture = graphics->CreateTexture(imageData, attributes);
		NONFATAL_ASSERT_RTRN(texture != NULL, "EngineObjectManager::CreateTexture -> Could not create new Texture object.", TextureRef::Null(), false);
		texture->SetObjectID(GetNextObjectID());

		return TextureRef(texture, [=](Texture * texture)
		{
			DeleteTexture(texture);
		});
	}

	TextureRef EngineObjectManager::CreateTexture(unsigned int width, unsigned int height, BYTE * pixelData, TextureAttributes attributes)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != NULL, "EngineObjectManager::CreateTexture -> Graphics system is null.");

		Texture * texture = graphics->CreateTexture(width, height, pixelData, attributes);
		NONFATAL_ASSERT_RTRN(texture != NULL, "EngineObjectManager::CreateTexture -> Could not create new Texture object.", TextureRef::Null(), false);
		texture->SetObjectID(GetNextObjectID());

		return TextureRef(texture, [=](Texture * texture)
		{
			DeleteTexture(texture);
		});
	}

	TextureRef EngineObjectManager::CreateCubeTexture(BYTE * frontData, unsigned int fw, unsigned int fh,
		BYTE * backData, unsigned int backw, unsigned int backh,
		BYTE * topData, unsigned int tw, unsigned int th,
		BYTE * bottomData, unsigned int botw, unsigned int both,
		BYTE * leftData, unsigned int lw, unsigned int lh,
		BYTE * rightData, unsigned int rw, unsigned int rh)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != NULL, "EngineObjectManager::CreateCubeTexture -> Graphics system is null.");

		Texture * texture = graphics->CreateCubeTexture(frontData, fw, fh,
			backData, backw, backh,
			topData, tw, th,
			bottomData, botw, both,
			leftData, lw, lh,
			rightData, rw, rh);

		NONFATAL_ASSERT_RTRN(texture != NULL, "EngineObjectManager::CreateCubeTexture -> Could not create new Texture object.", TextureRef::Null(), false);
		texture->SetObjectID(GetNextObjectID());

		return TextureRef(texture, [=](Texture * texture)
		{
			DeleteTexture(texture);
		});
	}

	TextureRef EngineObjectManager::CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
		const std::string& bottom, const std::string& left, const std::string& right)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != NULL, "EngineObjectManager::CreateCubeTexture -> Graphics system is null.");

		Texture * texture = graphics->CreateCubeTexture(front, back, top, bottom, left, right);
		NONFATAL_ASSERT_RTRN(texture != NULL, "EngineObjectManager::CreateCubeTexture -> Could not create new Texture object.", TextureRef::Null(), false);
		texture->SetObjectID(GetNextObjectID());

		return TextureRef(texture, [=](Texture * texture)
		{
			DeleteTexture(texture);
		});
	}

	TextureRef EngineObjectManager::CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
		RawImage * bottomData, RawImage * leftData, RawImage * rightData)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != NULL, "EngineObjectManager::CreateCubeTexture -> Graphics system is null.");

		Texture * texture = graphics->CreateCubeTexture(frontData, backData, topData, bottomData, leftData, rightData);
		NONFATAL_ASSERT_RTRN(texture != NULL, "EngineObjectManager::CreateCubeTexture -> Could create new Texture object.", TextureRef::Null(), false);
		texture->SetObjectID(GetNextObjectID());

		return TextureRef(texture, [=](Texture * texture)
		{
			DeleteTexture(texture);
		});
	}

	void EngineObjectManager::DestroyTexture(TextureRef texture)
	{
		texture.ForceDelete();
	}

	void EngineObjectManager::DeleteTexture(Texture * texture)
	{
		ASSERT(texture != NULL, "EngineObjectManager::DeleteTexture -> 'texture' is null.");

		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != NULL, "EngineObjectManager::DeleteTexture -> Graphics system is null.");

		graphics->DestroyTexture(texture);
	}

	RenderTargetRef EngineObjectManager::CreateRenderTarget(bool hasColor, bool hasDepth, bool enableStencilBuffer,
		const TextureAttributes& colorTextureAttributes, unsigned int width, unsigned int height)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != NULL, "EngineObjectManager::CreateRenderTarget -> Graphics system is null.");

		RenderTarget*  target = graphics->CreateRenderTarget(hasColor, hasDepth, enableStencilBuffer, colorTextureAttributes, width, height);
		NONFATAL_ASSERT_RTRN(target != NULL, "EngineObjectManager::CreateRenderBuffer -> Could not create new RenderTarget object.", RenderTargetRef::Null(), false);

		bool success = target->Init();
		NONFATAL_ASSERT_RTRN(success == true, "EngineObjectManager::CreateRenderBuffer -> Could not initialize RenderTarget object.", RenderTargetRef::Null(), false);

		return RenderTargetRef(target, [=](RenderTarget * target)
		{
			DeleteRenderTarget(target);
		});
	}

	RenderTargetRef EngineObjectManager::WrapRenderTarget(RenderTarget * target)
	{
		ASSERT(target != NULL, "EngineObjectManager::WrapRenderTarget -> 'target' is null.");

		return RenderTargetRef(target, [=](RenderTarget * target)
		{
			DeleteRenderTarget(target);
		});
	}

	void EngineObjectManager::DestroyRenderTarget(RenderTargetRef buffer)
	{
		buffer.ForceDelete();
	}

	void EngineObjectManager::DeleteRenderTarget(RenderTarget * target)
	{
		ASSERT(target != NULL, "EngineObjectManager::DeleteRenderBuffer -> 'target' is null.");

		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != NULL, "EngineObjectManager::DeleteRenderTarget -> Graphics system is null.");

		graphics->DestroyRenderTarget(target);
	}

	MaterialRef EngineObjectManager::CreateMaterial(const std::string& name, ShaderRef shader)
	{
		Material * m = new Material(name);
		ASSERT(m != NULL, "EngineObjectManager::CreateMaterial(std::string&, ShaderRef) -> Unable to allocate material.");

		bool initSuccess = m->Init(shader);
		if (!initSuccess)
		{
			Debug::PrintError("EngineObjectManager::CreateMaterial(std::string&, ShaderRef) -> could not Init material");
			delete m;
			return MaterialRef::Null();
		}
		m->SetObjectID(GetNextObjectID());

		return MaterialRef(m, [=](Material * m)
		{
			DeleteMaterial(m);
		});
	}

	MaterialRef EngineObjectManager::CreateMaterial(const std::string& name, const ShaderSource& shaderSource)
	{
		ShaderRef shader = CreateShader(shaderSource);
		if (!shader.IsValid())return MaterialRef::Null();

		Material * m = new Material(name);
		bool initSuccess = m->Init(shader);

		if (!initSuccess)
		{
			Debug::PrintError("EngineObjectManager::CreateMaterial(const std::string&, const std::string&) -> could not Init material");
			delete m;
			return MaterialRef::Null();
		}
		m->SetObjectID(GetNextObjectID());

		return MaterialRef(m, [=](Material * m)
		{
			DeleteMaterial(m);
		});
	}

	void EngineObjectManager::DestroyMaterial(MaterialRef material)
	{
		material.ForceDelete();
	}

	void EngineObjectManager::DeleteMaterial(Material * material)
	{
		ASSERT(material != NULL, "EngineObjectManager::DeleteMaterial -> 'material' is null.");
		ShaderRef shader = material->GetShader();

		ASSERT(shader.IsValid(), "EngineObjectManager::DeleteMaterial -> 'shader' is null.");
		DestroyShader(shader);

		delete material;
	}

	CameraRef EngineObjectManager::CreateCamera()
	{
		Camera * camera = new Camera();
		ASSERT(camera != NULL, "EngineObjectManager::CreateCamera -> Could not allocate new Camera object.");
		camera->SetObjectID(GetNextObjectID());

		LayerManager& layerManager = GetLayerManager();
		IntMask allMask = layerManager.CreateFullLayerMask();
		camera->SetCullingMask(allMask);

		return CameraRef(camera, [=](Camera * camera)
		{
			DeleteCamera(camera);
		});
	}

	void EngineObjectManager::DestroyCamera(CameraRef camera)
	{
		camera.ForceDelete();
	}

	void EngineObjectManager::DeleteCamera(Camera * camera)
	{
		ASSERT(camera != NULL, "EngineObjectManager::DeleteCamera -> 'camera' is null.");
		delete camera;
	}

	LightRef EngineObjectManager::CreateLight()
	{
		Light * light = new Light();
		ASSERT(light != NULL, "EngineObjectManager::CreateLight -> Could not create new Light object.");

		light->SetObjectID(GetNextObjectID());
		light->SetCullingMask(layerManager.GetLayerMask(DefaultLayer));

		return LightRef(light, [=](Light * light)
		{
			DeleteLight(light);
		});
	}

	void EngineObjectManager::DestroyLight(LightRef light)
	{
		light.ForceDelete();
	}

	void EngineObjectManager::DeleteLight(Light * light)
	{
		ASSERT(light != NULL, "EngineObjectManager::DeleteLight -> 'light' is null.");
		delete light;
	}
}

