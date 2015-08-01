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

	Bool EngineObjectManager::Init()
	{
		layerManager.AddLayer(DefaultLayer);
		return true;
	}

	Bool EngineObjectManager::InitBuiltinShaders()
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
		LongMaskUtil::SetBit(&shaderProperties, (Int16)ShaderMaterialCharacteristic::DiffuseColored);
		LongMaskUtil::SetBit(&shaderProperties, (Int16)ShaderMaterialCharacteristic::VertexNormals);
		loadedShaders.AddShader(shaderProperties, shader);

		assetImporter.LoadBuiltInShaderSource("diffuse_texture", shaderSource);
		shader = CreateShader(shaderSource);
		ASSERT(shader.IsValid(), "EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseTextured");
		shaderProperties = LongMaskUtil::CreateLongMask();
		LongMaskUtil::SetBit(&shaderProperties, (Int16)ShaderMaterialCharacteristic::DiffuseTextured);
		LongMaskUtil::SetBit(&shaderProperties, (Int16)ShaderMaterialCharacteristic::VertexNormals);
		loadedShaders.AddShader(shaderProperties, shader);

		assetImporter.LoadBuiltInShaderSource("diffuse_texture_vcolor", shaderSource);
		shader = CreateShader(shaderSource);
		ASSERT(shader.IsValid(), "EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseTextured & VertexColors");
		shaderProperties = LongMaskUtil::CreateLongMask();
		LongMaskUtil::SetBit(&shaderProperties, (Int16)ShaderMaterialCharacteristic::DiffuseTextured);
		LongMaskUtil::SetBit(&shaderProperties, (Int16)ShaderMaterialCharacteristic::VertexColors);
		LongMaskUtil::SetBit(&shaderProperties, (Int16)ShaderMaterialCharacteristic::VertexNormals);
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
		SceneObject *sceneObject = new(std::nothrow) SceneObject();
		ASSERT(sceneObject != nullptr, "EngineObjectManager::CreateSceneObject -> could not allocate new scene object.");
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
		ASSERT(sceneObject != nullptr, "EngineObjectManager::DeleteSceneObject -> sceneObject is null.");
		delete sceneObject;
	}

	const SceneObjectRef EngineObjectManager::GetSceneRoot() const
	{
		return (const SceneObjectRef)sceneRootRef;
	}

	Mesh3DRef EngineObjectManager::CreateMesh3D(UInt32 subMeshCount)
	{
		Mesh3D * mesh = new(std::nothrow) Mesh3D(subMeshCount);
		ASSERT(mesh != nullptr, "EngineObjectManager::CreateMesh3D -> Could not allocate new Mesh3D object.");
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
		ASSERT(mesh != nullptr, "EngineObjectManager::DeleteMesh -> 'mesh' is null.");
		delete mesh;
	}

	Mesh3DFilterRef EngineObjectManager::CreateMesh3DFilter()
	{
		Mesh3DFilter * filter = new(std::nothrow) Mesh3DFilter();
		ASSERT(filter != nullptr, "EngineObjectManager::CreateMesh3DFilter -> Could not allocate new Mesh3DFilter object.");
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
		ASSERT(filter != nullptr, "EngineObjectManager::DeleteMesh3DFilter -> 'filter' is null.");
		delete filter;
	}

	Mesh3DRendererRef EngineObjectManager::CreateMesh3DRenderer()
	{
		Mesh3DRenderer * renderer = new(std::nothrow) Mesh3DRenderer();
		ASSERT(renderer != nullptr, "EngineObjectManager::CreateMesh3DRenderer -> Could not allocate new Mesh3DRenderer object.");
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
		ASSERT(renderer != nullptr, "EngineObjectManager::DeleteMesh3DRenderer -> 'renderer' is null.");
		delete renderer;
	}

	SkinnedMesh3DRendererRef EngineObjectManager::CreateSkinnedMesh3DRenderer()
	{
		SkinnedMesh3DRenderer * renderer = new(std::nothrow) SkinnedMesh3DRenderer();
		ASSERT(renderer != nullptr, "EngineObjectManager::CreateSkinnedMesh3DRenderer -> Could not allocate new SkinnedMesh3DRenderer object.");
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
		ASSERT(renderer != nullptr, "EngineObjectManager::DeleteSkinnedMesh3DRenderer -> 'renderer' is null.");
		delete renderer;
	}

	SubMesh3DRef EngineObjectManager::CreateSubMesh3D(StandardAttributeSet attributes)
	{
		SubMesh3D * mesh = new(std::nothrow) SubMesh3D(attributes);
		ASSERT(mesh != nullptr, "EngineObjectManager::CreateSubMesh3D -> could not allocate new SubMesh3D object.");
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
		ASSERT(mesh != nullptr, "EngineObjectManager::DeleteSubMesh3D -> 'mesh' is null.");
		delete mesh;
	}

	SubMesh3DRendererRef  EngineObjectManager::CreateSubMesh3DRenderer()
	{
		return CreateSubMesh3DRenderer(nullptr);
	}

	SubMesh3DRendererRef EngineObjectManager::CreateSubMesh3DRenderer(AttributeTransformer * attrTransformer)
	{
		// TODO: for now we force vertex attribute buffers to be on GPU
		// in the constructor for SubMesh3DRenderer, need to think
		// of a better way to do this.
		SubMesh3DRenderer * renderer = new(std::nothrow) SubMesh3DRenderer(true, attrTransformer);

		ASSERT(renderer != nullptr, "EngineObjectManager::CreateMesh3DRenderer(AttributeTransformer) -> could not allocate new SubMesh3DRenderer object.");
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
		ASSERT(renderer != nullptr, "EngineObjectManager::DeleteSubMesh3DRenderer -> 'renderer' is null.");
		delete renderer;
	}

	SkeletonRef EngineObjectManager::CreateSkeleton(UInt32 boneCount)
	{
		Skeleton * skeleton = new(std::nothrow) Skeleton(boneCount);
		ASSERT(skeleton != nullptr, "EngineObjectManager::CreateSkeleton -> Could not allocate new skeleton.");

		return SkeletonRef(skeleton, [=](Skeleton * skeleton)
		{
			DeleteSkeleton(skeleton);
		});
	}

	SkeletonRef EngineObjectManager::CloneSkeleton(SkeletonRef source)
	{
		NONFATAL_ASSERT_RTRN(source.IsValid(), "EngineObjectManager::CloneSkeleton -> 'source' is invalid.", SkeletonRef::Null(), true);

		Skeleton * skeleton = source->FullClone();
		NONFATAL_ASSERT_RTRN(skeleton != nullptr, "EngineObjectManager::CloneSkeleton -> Could not clone source.", SkeletonRef::Null(), true);

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
		ASSERT(target != nullptr, "EngineObjectManager::DeleteSkeleton -> 'skeleton' is null.");
		delete target;
	}

	AnimationRef EngineObjectManager::CreateAnimation(Real duration, Real ticksPerSecond)
	{
		Animation * animation = new(std::nothrow) Animation(duration, ticksPerSecond);
		ASSERT(animation != nullptr, "EngineObjectManager::CreateAnimation -> Could not allocate new Animation object.");
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
		ASSERT(animation != nullptr, "EngineObjectManager::CreateAnimation -> 'animation' is null.");
		delete animation;
	}

	AnimationInstanceRef EngineObjectManager::CreateAnimationInstance(SkeletonRef target, AnimationRefConst animation)
	{
		NONFATAL_ASSERT_RTRN(target.IsValid(), "EngineObjectManager::CreateAnimationInstance -> 'target' is invalid.", AnimationInstanceRef::Null(), true);
		NONFATAL_ASSERT_RTRN(animation.IsValid(), "EngineObjectManager::CreateAnimationInstance -> 'animation' is invalid.", AnimationInstanceRef::Null(), true);

		AnimationInstance * instance = new(std::nothrow) AnimationInstance(target, animation);
		ASSERT(instance != nullptr, "EngineObjectManager::CreateAnimationInstance -> Could not allocate new AnimationInstance object.");

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
		ASSERT(instance != nullptr, "EngineObjectManager::DeleteAnimationInstance -> 'instance' is null.");
		delete instance;
	}

	AnimationPlayerRef EngineObjectManager::CreateAnimationPlayer(SkeletonRef target)
	{
		NONFATAL_ASSERT_RTRN(target.IsValid(), "EngineObjectManager::CreateAnimationPlayer -> 'target' is invalid.", AnimationPlayerRef::Null(), true);

		AnimationPlayer * player = new(std::nothrow) AnimationPlayer(target);
		ASSERT(player != nullptr, "EngineObjectManager::CreateAnimationPlayer -> Could not allocate new AnimationPlayer object.");

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
		ASSERT(player != nullptr, "EngineObjectManager::DeleteAnimationPlayer -> 'player' is null.");
		delete player;
	}

	ShaderRef EngineObjectManager::CreateShader(const ShaderSource& shaderSource)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != nullptr, "EngineObjectManager::CreateShader -> Graphics system is null.");

		Shader * shader = graphics->CreateShader(shaderSource);
		NONFATAL_ASSERT_RTRN(shader != nullptr, "EngineObjectManager::CreateShader -> Could not create new Shader object.", ShaderRef::Null(), false);
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
		ASSERT(graphics != nullptr, "EngineObjectManager::DeleteShader -> Graphics system is null.");

		ASSERT(shader != nullptr, "EngineObjectManager::DeleteShader -> 'shader' is null.");
		graphics->DestroyShader(shader);
	}

	TextureRef EngineObjectManager::CreateTexture(const std::string& sourcePath, TextureAttributes attributes)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		Texture * texture = graphics->CreateTexture(sourcePath, attributes);

		if (texture == nullptr)
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
		ASSERT(graphics != nullptr, "EngineObjectManager::CreateTexture -> Graphics system is null.");

		Texture * texture = graphics->CreateTexture(imageData, attributes);
		NONFATAL_ASSERT_RTRN(texture != nullptr, "EngineObjectManager::CreateTexture -> Could not create new Texture object.", TextureRef::Null(), false);
		texture->SetObjectID(GetNextObjectID());

		return TextureRef(texture, [=](Texture * texture)
		{
			DeleteTexture(texture);
		});
	}

	TextureRef EngineObjectManager::CreateTexture(UInt32 width, UInt32 height, Byte * pixelData, TextureAttributes attributes)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != nullptr, "EngineObjectManager::CreateTexture -> Graphics system is null.");

		Texture * texture = graphics->CreateTexture(width, height, pixelData, attributes);
		NONFATAL_ASSERT_RTRN(texture != nullptr, "EngineObjectManager::CreateTexture -> Could not create new Texture object.", TextureRef::Null(), false);
		texture->SetObjectID(GetNextObjectID());

		return TextureRef(texture, [=](Texture * texture)
		{
			DeleteTexture(texture);
		});
	}

	TextureRef EngineObjectManager::CreateCubeTexture(Byte * frontData, UInt32 fw, UInt32 fh,
		Byte * backData, UInt32 backw, UInt32 backh,
		Byte * topData, UInt32 tw, UInt32 th,
		Byte * bottomData, UInt32 botw, UInt32 both,
		Byte * leftData, UInt32 lw, UInt32 lh,
		Byte * rightData, UInt32 rw, UInt32 rh)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != nullptr, "EngineObjectManager::CreateCubeTexture -> Graphics system is null.");

		Texture * texture = graphics->CreateCubeTexture(frontData, fw, fh,
			backData, backw, backh,
			topData, tw, th,
			bottomData, botw, both,
			leftData, lw, lh,
			rightData, rw, rh);

		NONFATAL_ASSERT_RTRN(texture != nullptr, "EngineObjectManager::CreateCubeTexture -> Could not create new Texture object.", TextureRef::Null(), false);
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
		ASSERT(graphics != nullptr, "EngineObjectManager::CreateCubeTexture -> Graphics system is null.");

		Texture * texture = graphics->CreateCubeTexture(front, back, top, bottom, left, right);
		NONFATAL_ASSERT_RTRN(texture != nullptr, "EngineObjectManager::CreateCubeTexture -> Could not create new Texture object.", TextureRef::Null(), false);
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
		ASSERT(graphics != nullptr, "EngineObjectManager::CreateCubeTexture -> Graphics system is null.");

		Texture * texture = graphics->CreateCubeTexture(frontData, backData, topData, bottomData, leftData, rightData);
		NONFATAL_ASSERT_RTRN(texture != nullptr, "EngineObjectManager::CreateCubeTexture -> Could create new Texture object.", TextureRef::Null(), false);
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
		ASSERT(texture != nullptr, "EngineObjectManager::DeleteTexture -> 'texture' is null.");

		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != nullptr, "EngineObjectManager::DeleteTexture -> Graphics system is null.");

		graphics->DestroyTexture(texture);
	}

	RenderTargetRef EngineObjectManager::CreateRenderTarget(Bool hasColor, Bool hasDepth, Bool enableStencilBuffer,
		const TextureAttributes& colorTextureAttributes, UInt32 width, UInt32 height)
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != nullptr, "EngineObjectManager::CreateRenderTarget -> Graphics system is null.");

		RenderTarget*  target = graphics->CreateRenderTarget(hasColor, hasDepth, enableStencilBuffer, colorTextureAttributes, width, height);
		NONFATAL_ASSERT_RTRN(target != nullptr, "EngineObjectManager::CreateRenderBuffer -> Could not create new RenderTarget object.", RenderTargetRef::Null(), false);

		Bool success = target->Init();
		NONFATAL_ASSERT_RTRN(success == true, "EngineObjectManager::CreateRenderBuffer -> Could not initialize RenderTarget object.", RenderTargetRef::Null(), false);

		return RenderTargetRef(target, [=](RenderTarget * target)
		{
			DeleteRenderTarget(target);
		});
	}

	RenderTargetRef EngineObjectManager::WrapRenderTarget(RenderTarget * target)
	{
		ASSERT(target != nullptr, "EngineObjectManager::WrapRenderTarget -> 'target' is null.");

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
		ASSERT(target != nullptr, "EngineObjectManager::DeleteRenderBuffer -> 'target' is null.");

		Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
		ASSERT(graphics != nullptr, "EngineObjectManager::DeleteRenderTarget -> Graphics system is null.");

		graphics->DestroyRenderTarget(target);
	}

	MaterialRef EngineObjectManager::CreateMaterial(const std::string& name, ShaderRef shader)
	{
		Material * m = new(std::nothrow) Material(name);
		ASSERT(m != nullptr, "EngineObjectManager::CreateMaterial(std::string&, ShaderRef) -> Unable to allocate material.");

		Bool initSuccess = m->Init(shader);
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

		Material * m = new(std::nothrow) Material(name);
		Bool initSuccess = m->Init(shader);

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
		ASSERT(material != nullptr, "EngineObjectManager::DeleteMaterial -> 'material' is null.");
		ShaderRef shader = material->GetShader();

		ASSERT(shader.IsValid(), "EngineObjectManager::DeleteMaterial -> 'shader' is null.");
		DestroyShader(shader);

		delete material;
	}

	CameraRef EngineObjectManager::CreateCamera()
	{
		Camera * camera = new(std::nothrow) Camera();
		ASSERT(camera != nullptr, "EngineObjectManager::CreateCamera -> Could not allocate new Camera object.");
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
		ASSERT(camera != nullptr, "EngineObjectManager::DeleteCamera -> 'camera' is null.");
		delete camera;
	}

	LightRef EngineObjectManager::CreateLight()
	{
		Light * light = new(std::nothrow) Light();
		ASSERT(light != nullptr, "EngineObjectManager::CreateLight -> Could not create new Light object.");

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
		ASSERT(light != nullptr, "EngineObjectManager::DeleteLight -> 'light' is null.");
		delete light;
	}
}

