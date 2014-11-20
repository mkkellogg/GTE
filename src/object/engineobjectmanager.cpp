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
#include "graphics/stdattributes.h"
#include "graphics/texture/texture.h"
#include "graphics/image/rawimage.h"
#include "graphics/animation/skeleton.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/animationinstance.h"
#include "graphics/animation/animationplayer.h"
#include "ui/debug.h"
#include "graphics/view/camera.h"
#include "base/longmask.h"
#include "util/util.h"
#include <string>

EngineObjectManager::EngineObjectManager()
{
	currentEngineObjectID = 0L;

	sceneRoot.SetObjectID(GetNextObjectID());
	sceneRootRef = 	SceneObjectRef(&sceneRoot, [=](SceneObject * sceneObject)
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

bool EngineObjectManager::InitBuiltinShaders()
{
	std::string vertexSource;
	std::string fragmentSource;
	ShaderRef shader;

	LongMask shaderProperties;
	vertexSource = std::string(builtinPath) + std::string("diffuse.vertex.shader");
	fragmentSource = std::string(builtinPath) + std::string("diffuse.fragment.shader");
	shader = CreateShader(vertexSource.c_str(),fragmentSource.c_str());
	ASSERT(shader.IsValid(),"EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseColored", false);
	shaderProperties = LongMaskUtil::CreateLongMask();
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::DiffuseColored);
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexNormals);
	loadedShaders.AddShader(shaderProperties,shader);

	vertexSource = std::string(builtinPath) + std::string("diffuse_texture.vertex.shader");
	fragmentSource = std::string(builtinPath) + std::string("diffuse_texture.fragment.shader");
	shader = CreateShader(vertexSource.c_str(),fragmentSource.c_str());
	ASSERT(shader.IsValid(),"EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseTextured", false);
	shaderProperties = LongMaskUtil::CreateLongMask();
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::DiffuseTextured);
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexNormals);
	loadedShaders.AddShader(shaderProperties,shader);

	vertexSource = std::string(builtinPath) + std::string("diffuse_texture_vcolor.vertex.shader");
	fragmentSource = std::string(builtinPath) + std::string("diffuse_texture_vcolor.fragment.shader");
	shader = CreateShader(vertexSource.c_str(),fragmentSource.c_str());
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

SceneObjectRef EngineObjectManager::CreateSceneObject()
{
	SceneObject *sceneObject = new SceneObject();
	ASSERT(sceneObject != NULL,"EngineObjectManager::CreateSceneObject -> could not allocate new scene object.", SceneObjectRef::Null());
	sceneObject->SetObjectID(GetNextObjectID());

	SceneObjectRef ref(sceneObject, [=](SceneObject * sceneObject)
	{
		  DeleteSceneObject(sceneObject);
	});

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
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	SubMesh3DRenderer * renderer = graphics->CreateMeshRenderer(attrTransformer);

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
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	ASSERT_RTRN(renderer != NULL,"EngineObjectManager::DeleteSubMesh3DRenderer -> renderer is NULL.");
	graphics->DestroyMeshRenderer(renderer);
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

AnimationRef EngineObjectManager::CreateAnimation(float duration, float ticksPerSecond, SkeletonRef skeleton)
{
	Animation * animation = new Animation(duration, ticksPerSecond, skeleton);
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

ShaderRef EngineObjectManager::CreateShader(const char * vertexSourcePath, const char * fragmentSourcePath)
{
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	Shader * shader = graphics->CreateShader(vertexSourcePath,fragmentSourcePath);
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

TextureRef EngineObjectManager::CreateTexture(const char * sourcePath, TextureAttributes attributes)
{
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	Texture * texture = graphics->CreateTexture(sourcePath, attributes);
	ASSERT(texture != NULL,"EngineObjectManager::CreateTexture(const char *, TextureAttributes) -> couldn't create new Texture object.", TextureRef::Null());
	texture->SetObjectID(GetNextObjectID());

	return TextureRef(texture, [=](Texture * texture)
	{
		  DeleteTexture(texture);
	});
}

TextureRef EngineObjectManager::CreateTexture(const RawImage * imageData, const char * sourcePath, TextureAttributes attributes)
{
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	Texture * texture = graphics->CreateTexture(imageData, sourcePath, attributes);
	ASSERT(texture != NULL,"EngineObjectManager::CreateTexture(const RawImage*, const char *, TextureAttributes) -> could create new Texture object.", TextureRef::Null());
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

MaterialRef EngineObjectManager::CreateMaterial(const char *name, ShaderRef shader)
{
	Material * m = new Material(name);
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

MaterialRef EngineObjectManager::CreateMaterial(const char *name, const char * shaderVertexSourcePath, const char * shaderFragmentSourcePath)
{
	ShaderRef shader= CreateShader(shaderVertexSourcePath, shaderFragmentSourcePath);
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
	Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
	Camera * camera = new Camera(graphics);
	ASSERT(camera != NULL, "EngineObjectManager::CreateCamera -> Could not create new Camera object.", CameraRef::Null());
	camera->SetObjectID(GetNextObjectID());

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

