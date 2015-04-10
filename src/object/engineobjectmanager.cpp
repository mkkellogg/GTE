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
	std::string builtinPath = fileSystem->GetPathFromIXPath(Constants::BuiltinShaderPath);

	LongMask shaderProperties;

	assetImporter.LoadBuiltInShaderSource("diffuse", shaderSource);
	shader = CreateShader(shaderSource);
	ASSERT(shader.IsValid(),"EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseColored", false);
	shaderProperties = LongMaskUtil::CreateLongMask();
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::DiffuseColored);
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexNormals);
	loadedShaders.AddShader(shaderProperties,shader);

	assetImporter.LoadBuiltInShaderSource("diffuse_texture", shaderSource);
	shader = CreateShader(shaderSource);
	ASSERT(shader.IsValid(),"EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseTextured", false);
	shaderProperties = LongMaskUtil::CreateLongMask();
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::DiffuseTextured);
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexNormals);
	loadedShaders.AddShader(shaderProperties,shader);

	assetImporter.LoadBuiltInShaderSource("diffuse_texture_vcolor", shaderSource);
	shader = CreateShader(shaderSource);
	ASSERT(shader.IsValid(),"EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseTextured & VertexColors", false);
	shaderProperties = LongMaskUtil::CreateLongMask();
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::DiffuseTextured);
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexColors);
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexNormals);
	loadedShaders.AddShader(shaderProperties,shader);

	return true;
}

SceneObjectRef EngineObjectManager::FindSceneObjectInDirectory(unsigned long objectID)
{
	if(sceneObjectDirectory.find(objectID) != sceneObjectDirectory.end())
	{
		SceneObjectRef ref = sceneObjectDirectory[objectID];
		return ref;
	}

	return SceneObjectRef::Null();
}

void EngineObjectManager::AddSceneObjectToDirectory(unsigned long objectID, SceneObjectRef ref)
{
	ASSERT_RTRN(ref.IsValid(), "EngineObjectManager::AddSceneObjectToDirectory -> Tried to add NULL scene object reference.");
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
	ASSERT(sceneObject != NULL,"EngineObjectManager::CreateSceneObject -> could not allocate new scene object.", SceneObjectRef::Null());
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
	sceneObject.ForceDelete();
}

void EngineObjectManager::DeleteSceneObject(SceneObject * sceneObject)
{
	ASSERT_RTRN(sceneObject!=NULL, "EngineObjectManager::DeleteSceneObject -> sceneObject is NULL.");
	delete sceneObject;
}

const SceneObjectRef EngineObjectManager::GetSceneRoot() const
{
	return (const SceneObjectRef)sceneRootRef;
}

Mesh3DRef EngineObjectManager::CreateMesh3D(unsigned int subMeshCount)
{
	Mesh3D * mesh =  new Mesh3D(subMeshCount);
	ASSERT(mesh != NULL,"EngineObjectManager::CreateMesh3D -> could not create new Mesh3D object.", Mesh3DRef::Null());
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
	ASSERT_RTRN(mesh != NULL, "EngineObjectManager::DeleteMesh -> mesh is NULL.");
	delete mesh;
}

Mesh3DFilterRef EngineObjectManager::CreateMesh3DFilter()
{
	Mesh3DFilter * filter =  new Mesh3DFilter();
	ASSERT(filter != NULL,"EngineObjectManager::CreateMesh3DFilter -> could not create new Mesh3DFilter object.", Mesh3DFilterRef::Null());
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
	ASSERT_RTRN(filter != NULL, "EngineObjectManager::DeleteMesh3DFilter -> filter is NULL.");
	delete filter;
}

Mesh3DRendererRef EngineObjectManager::CreateMesh3DRenderer()
{
	Mesh3DRenderer * renderer =  new Mesh3DRenderer();
	ASSERT(renderer != NULL,"EngineObjectManager::CreateMesh3DRenderer -> could not create new Mesh3DRenderer object.", Mesh3DRendererRef::Null());
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
	ASSERT_RTRN(renderer!= NULL,"EngineObjectManager::DeleteMesh3DRenderer -> renderer is NULL.");
	delete renderer;
}

SkinnedMesh3DRendererRef EngineObjectManager::CreateSkinnedMesh3DRenderer()
{
	SkinnedMesh3DRenderer * renderer =  new SkinnedMesh3DRenderer();
	ASSERT(renderer != NULL,"EngineObjectManager::CreateSkinnedMesh3DRenderer -> could not create new SkinnedMesh3DRenderer object.", SkinnedMesh3DRendererRef::Null());
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
	ASSERT_RTRN(renderer != NULL,"EngineObjectManager::DeleteSkinnedMesh3DRenderer -> renderer is NULL.");
	delete renderer;
}

SubMesh3DRef EngineObjectManager::CreateSubMesh3D(StandardAttributeSet attributes)
{
	SubMesh3D * mesh = new SubMesh3D(attributes);
	ASSERT(mesh != NULL,"EngineObjectManager::CreateSubMesh3D -> could not create new SubMesh3D object.", SubMesh3DRef::Null());
	mesh->SetObjectID(GetNextObjectID());

	return SubMesh3DRef(mesh, [=](SubMesh3D * mesh)
	{
		  DeleteSubMesh3D(mesh);
	});
}

void EngineObjectManager::DestroySubMesh3D(SubMesh3DRef mesh)
{
	mesh.ForceDelete();
}

void EngineObjectManager::DeleteSubMesh3D(SubMesh3D * mesh)
{
	ASSERT_RTRN(mesh != NULL,"EngineObjectManager::DeleteSubMesh3D -> mesh is NULL.");
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

	ASSERT(renderer != NULL,"EngineObjectManager::CreateMesh3DRenderer(AttributeTransformer) -> could not create new SubMesh3DRenderer object.", SubMesh3DRendererRef::Null());
	renderer->SetObjectID(GetNextObjectID());

	return SubMesh3DRendererRef(renderer, [=](SubMesh3DRenderer * renderer)
	{
		  DeleteSubMesh3DRenderer(renderer);
	});
}

void EngineObjectManager::DestroySubMesh3DRenderer(SubMesh3DRendererRef renderer)
{
	renderer.ForceDelete();
}

void EngineObjectManager::DeleteSubMesh3DRenderer(SubMesh3DRenderer * renderer)
{
	ASSERT_RTRN(renderer != NULL,"EngineObjectManager::DeleteSubMesh3DRenderer -> renderer is NULL.");

	delete renderer;
}

SkeletonRef EngineObjectManager::CreateSkeleton(unsigned int boneCount)
{
	Skeleton * skeleton = new Skeleton(boneCount);
	ASSERT(skeleton != NULL,"EngineObjectManager::CreateSkeleton -> Could not allocate new skeleton.", SkeletonRef::Null());

	return SkeletonRef(skeleton, [=](Skeleton * skeleton)
	{
		DeleteSkeleton(skeleton);
	});
}

SkeletonRef EngineObjectManager::CloneSkeleton(SkeletonRef source)
{
	ASSERT(source.IsValid(),"EngineObjectManager::CloneSkeleton -> source is invalid.", SkeletonRef::Null());

	Skeleton * skeleton = source->FullClone();
	ASSERT(skeleton != NULL,"EngineObjectManager::CloneSkeleton -> Could not clone source.", SkeletonRef::Null());

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
	ASSERT_RTRN(target != NULL, "EngineObjectManager::DeleteSkeleton -> skeleton is NULL.");
	delete target;
}

AnimationRef EngineObjectManager::CreateAnimation(float duration, float ticksPerSecond)
{
	Animation * animation = new Animation(duration, ticksPerSecond);
	ASSERT(animation != NULL, "EngineObjectManager::CreateAnimation -> Could not create new Animation object.", AnimationRef::Null());
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
	ASSERT_RTRN(animation != NULL, "EngineObjectManager::CreateAnimation -> animation is NULL.");
	delete animation;
}

AnimationInstanceRef EngineObjectManager::CreateAnimationInstance(SkeletonRef target, AnimationRef animation)
{
	ASSERT(target.IsValid(), "EngineObjectManager::CreateAnimationInstance -> target is invalid.", AnimationInstanceRef::Null());
	ASSERT(animation.IsValid(), "EngineObjectManager::CreateAnimationInstance -> animation is invalid.", AnimationInstanceRef::Null());

	AnimationInstance * instance = new AnimationInstance(target, animation);
	ASSERT(instance != NULL, "EngineObjectManager::CreateAnimationInstance -> Could not create new AnimationInstance object.", AnimationInstanceRef::Null());

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
	ASSERT_RTRN(instance != NULL, "EngineObjectManager::DeleteAnimationInstance -> instance is NULL.");
	delete instance;
}

AnimationPlayerRef EngineObjectManager::CreateAnimationPlayer(SkeletonRef target)
{
	ASSERT(target.IsValid(), "EngineObjectManager::CreateAnimationPlayer -> target is invalid.", AnimationPlayerRef::Null());

	AnimationPlayer * player = new AnimationPlayer(target);
	ASSERT(player != NULL, "EngineObjectManager::CreateAnimationPlayer -> Could not create new AnimationPlayer object.", AnimationPlayerRef::Null());

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
	ASSERT_RTRN(player != NULL, "EngineObjectManager::DeleteAnimationPlayer -> player is NULL.");
	delete player;
}

ShaderRef EngineObjectManager::CreateShader(const ShaderSource& shaderSource)
{
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	Shader * shader = graphics->CreateShader(shaderSource);
	ASSERT(shader != NULL,"EngineObjectManager::CreateShader -> could not create new Shader object.", ShaderRef::Null());
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
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	ASSERT_RTRN(shader != NULL,"EngineObjectManager::DeleteShader -> shader is NULL.");
	graphics->DestroyShader(shader);
}

TextureRef EngineObjectManager::CreateTexture(const std::string& sourcePath, TextureAttributes attributes)
{
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	Texture * texture = graphics->CreateTexture(sourcePath, attributes);

	if(texture == NULL)
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
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	Texture * texture = graphics->CreateTexture(imageData, attributes);
	ASSERT(texture != NULL,"EngineObjectManager::CreateTexture -> could not create new Texture object.", TextureRef::Null());
	texture->SetObjectID(GetNextObjectID());

	return TextureRef(texture, [=](Texture * texture)
	{
		  DeleteTexture(texture);
	});
}

TextureRef EngineObjectManager::CreateTexture(unsigned int width, unsigned int height, BYTE * pixelData, TextureAttributes attributes)
{
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	Texture * texture = graphics->CreateTexture(width, height, pixelData, attributes);
	ASSERT(texture != NULL,"EngineObjectManager::CreateTexture -> could not create new Texture object.", TextureRef::Null());
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
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	Texture * texture = graphics->CreateCubeTexture(frontData, fw,fh,
													backData, backw, backh,
													topData, tw, th,
													bottomData, botw, both,
													leftData, lw, lh,
													rightData, rw, rh);
	ASSERT(texture != NULL,"EngineObjectManager::CreateCubeTexture -> could not create new Texture object.", TextureRef::Null());
	texture->SetObjectID(GetNextObjectID());

	return TextureRef(texture, [=](Texture * texture)
	{
		  DeleteTexture(texture);
	});
}

TextureRef EngineObjectManager::CreateCubeTexture(const std::string& front, const std::string& back, const std::string& top,
		    				 	 	 	 	 	  const std::string& bottom, const std::string& left, const std::string& right)
{
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	Texture * texture = graphics->CreateCubeTexture(front, back, top, bottom, left, right);
	ASSERT(texture != NULL,"EngineObjectManager::CreateCubeTexture -> could not create new Texture object.", TextureRef::Null());
	texture->SetObjectID(GetNextObjectID());

	return TextureRef(texture, [=](Texture * texture)
	{
		  DeleteTexture(texture);
	});
}

TextureRef EngineObjectManager::CreateCubeTexture(RawImage * frontData, RawImage * backData, RawImage * topData,
							 	 	 	 	      RawImage * bottomData, RawImage * leftData, RawImage * rightData)
{
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	Texture * texture = graphics->CreateCubeTexture(frontData, backData, topData, bottomData, leftData, rightData);
	ASSERT(texture != NULL,"EngineObjectManager::CreateCubeTexture -> could create new Texture object.", TextureRef::Null());
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
	ASSERT_RTRN(texture != NULL,"EngineObjectManager::DeleteTexture -> texture is NULL.");

	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	graphics->DestroyTexture(texture);
}

RenderTargetRef EngineObjectManager::CreateRenderTarget(bool hasColor, bool hasDepth, bool enableStencilBuffer,
														const TextureAttributes& colorTextureAttributes, unsigned int width, unsigned int height)
{
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();

	RenderTarget*  target = graphics->CreateRenderTarget(hasColor, hasDepth, enableStencilBuffer, colorTextureAttributes, width, height);
	ASSERT(target != NULL, "EngineObjectManager::CreateRenderBuffer -> Could not create new RenderTarget object.", RenderTargetRef::Null());

	bool success = target->Init();
	ASSERT(success == true, "EngineObjectManager::CreateRenderBuffer -> Could not initialize RenderTarget object.", RenderTargetRef::Null());

	return RenderTargetRef(target, [=](RenderTarget * target)
	{
		  DeleteRenderTarget(target);
	});
}

RenderTargetRef EngineObjectManager::WrapRenderTarget(RenderTarget * target)
{
	ASSERT(target != NULL, "EngineObjectManager::WrapRenderTarget -> target is NULL.", RenderTargetRef::Null());

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
	ASSERT_RTRN(target != NULL,"EngineObjectManager::DeleteRenderBuffer -> target is NULL.");

	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	graphics->DestroyRenderTarget(target);
}

MaterialRef EngineObjectManager::CreateMaterial(const std::string& name, ShaderRef shader)
{
	Material * m = new Material(name);
	ASSERT(m != NULL, "EngineObjectManager::CreateMaterial -> Unable to allocate material.", MaterialRef::Null());

	bool initSuccess = m->Init(shader);
	if(!initSuccess)
	{
		Debug::PrintError("EngineObjectManager::CreateMaterial(Shader *) -> could not Init material");
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
	ShaderRef shader= CreateShader(shaderSource);
	if(!shader.IsValid())return MaterialRef::Null();

	Material * m = new Material(name);
	bool initSuccess = m->Init(shader);

	if(!initSuccess)
	{
		Debug::PrintError("EngineObjectManager::CreateMaterial(const char *, const char *) -> could not Init material");
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
	ASSERT_RTRN(material != NULL,"EngineObjectManager::DeleteMaterial -> material is NULL.");
	ShaderRef shader = material->GetShader();

	ASSERT_RTRN(shader.IsValid(),"EngineObjectManager::DeleteMaterial -> shader is NULL.");
	DestroyShader(shader);

	delete material;
}

CameraRef EngineObjectManager::CreateCamera()
{
	Camera * camera = new Camera();
	ASSERT(camera != NULL, "EngineObjectManager::CreateCamera -> Could not create new Camera object.", CameraRef::Null());
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
	ASSERT_RTRN(camera != NULL, "EngineObjectManager::DeleteCamera -> camera is NULL.");
	delete camera;
}

LightRef EngineObjectManager::CreateLight()
{
	Light * light = new Light();
	ASSERT(light != NULL, "EngineObjectManager::CreateLight -> Could not create new Light object.", LightRef::Null());

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
	ASSERT_RTRN(light != NULL, "EngineObjectManager::DeleteLight -> light is NULL.");
	delete light;
}

