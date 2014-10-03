#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engineobjectmanager.h"
#include "sceneobject.h"
#include "enginetypes.h"
#include "graphics/graphics.h"
#include "graphics/shader/shader.h"
#include "graphics/light/light.h"
#include "graphics/render/material.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/attributetransformer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/object/mesh3D.h"
#include "graphics/stdattributes.h"
#include "graphics/texture/texture.h"
#include "graphics/image/rawimage.h"
#include "ui/debug.h"
#include "graphics/view/camera.h"
#include "base/longmask.h"
#include <string>

EngineObjectManager * EngineObjectManager::theInstance = NULL;

EngineObjectManager * EngineObjectManager::Instance()
{
	//TODO: make thread-safe & add double checked locking
    if(theInstance == NULL)
    {
        theInstance = new EngineObjectManager();
    }

    return theInstance;
}

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
	Graphics * graphics = Graphics::Instance();
	std::string vertexSource;
	std::string fragmentSource;
	Shader * shader = NULL;

	LongMask shaderProperties;
	vertexSource = std::string(builtinPath) + std::string("diffuse.vertex.shader");
	fragmentSource = std::string(builtinPath) + std::string("diffuse.fragment.shader");
	shader = graphics->CreateShader(vertexSource.c_str(),fragmentSource.c_str());
	if(shader == NULL)
	{
		Debug::PrintError("EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseColored");
		return false;
	}
	shaderProperties = LongMaskUtil::CreateLongMask();
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::DiffuseColored);
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexNormals);
	loadedShaders.AddShader(shaderProperties,shader);

	vertexSource = std::string(builtinPath) + std::string("diffuse_texture.vertex.shader");
	fragmentSource = std::string(builtinPath) + std::string("diffuse_texture.fragment.shader");
	shader = graphics->CreateShader(vertexSource.c_str(),fragmentSource.c_str());
	if(shader == NULL)
	{
		Debug::PrintError("EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseTextured");
		return false;
	}
	shaderProperties = LongMaskUtil::CreateLongMask();
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::DiffuseTextured);
	LongMaskUtil::SetBit(&shaderProperties, (short)ShaderMaterialCharacteristic::VertexNormals);
	loadedShaders.AddShader(shaderProperties,shader);

	vertexSource = std::string(builtinPath) + std::string("diffuse_texture_vcolor.vertex.shader");
	fragmentSource = std::string(builtinPath) + std::string("diffuse_texture_vcolor.fragment.shader");
	shader = graphics->CreateShader(vertexSource.c_str(),fragmentSource.c_str());
	if(shader == NULL)
	{
		Debug::PrintError("EngineObjectManager::InitBuiltinShaders -> could not create builtin shader: DiffuseTextured & VertexColors");
		return false;
	}
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
	SHARED_REF_CHECK_RTRN(ref, "EngineObjectManager::AddSceneObjectToDirectory -> Tried to add NULL scene object reference.");

	sceneObjectDirectory[objectID] = ref;
}

Shader *  EngineObjectManager::GetLoadedShader(LongMask properties)
{
	return loadedShaders.GetShader(properties);
}

SceneObjectRef EngineObjectManager::CreateSceneObject()
{
	SceneObject *sceneObject = new SceneObject();
	NULL_CHECK(sceneObject,"EngineObjectManager::CreateSceneObject -> could not allocate new scene object.", SceneObjectRef::Null());
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
	NULL_CHECK_RTRN(sceneObject, "EngineObjectManager::DeleteSceneObject -> sceneObject is NULL.");
	delete sceneObject;
}

const SceneObjectRef EngineObjectManager::GetSceneRoot() const
{
	return (const SceneObjectRef)sceneRootRef;
}

Mesh3DRef EngineObjectManager::CreateMesh3D(unsigned int subMeshCount)
{
	Mesh3D * mesh =  new Mesh3D(subMeshCount);
	NULL_CHECK(mesh,"EngineObjectManager::CreateMesh3D -> could not create new Mesh3D object.", Mesh3DRef::Null());
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
	NULL_CHECK_RTRN(mesh, "EngineObjectManager::DeleteMesh -> mesh is NULL.");
	delete mesh;
}

Mesh3DRendererRef EngineObjectManager::CreateMesh3DRenderer()
{
	Mesh3DRenderer * renderer =  new Mesh3DRenderer();
	NULL_CHECK(renderer,"EngineObjectManager::CreateMesh3DRenderer -> could not create new Mesh3DRenderer object.", Mesh3DRendererRef::Null());
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

void EngineObjectManager::DeleteMesh3DRenderer(Mesh3DRenderer* renderer)
{
	NULL_CHECK_RTRN(renderer,"EngineObjectManager::DeleteMesh3DRenderer -> renderer is NULL.");
	delete renderer;
}

SubMesh3DRef EngineObjectManager::CreateSubMesh3D(StandardAttributeSet attributes)
{
	SubMesh3D * mesh = new SubMesh3D(attributes);
	NULL_CHECK(mesh,"EngineObjectManager::CreateSubMesh3D -> could not create new SubMesh3D object.", SubMesh3DRef::Null());
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
	NULL_CHECK_RTRN(mesh,"EngineObjectManager::DeleteSubMesh3D -> mesh is NULL.");
	delete mesh;
}

SubMesh3DRendererRef  EngineObjectManager::CreateSubMesh3DRenderer()
{
	return CreateSubMesh3DRenderer(NULL);
}

SubMesh3DRendererRef EngineObjectManager::CreateSubMesh3DRenderer(AttributeTransformer * attrTransformer)
{
	Graphics * graphics = Graphics::Instance();

	SubMesh3DRenderer * renderer = graphics->CreateMeshRenderer(attrTransformer);

	NULL_CHECK(renderer,"EngineObjectManager::CreateMesh3DRenderer(AttributeTransformer) -> could not create new SubMesh3DRenderer object.", SubMesh3DRendererRef::Null());
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
	Graphics * graphics = Graphics::Instance();
	NULL_CHECK_RTRN(renderer,"EngineObjectManager::DeleteSubMesh3DRenderer -> renderer is NULL.");
	graphics->DestroyMeshRenderer(renderer);
}

Shader * EngineObjectManager::CreateShader(const char * vertexSourcePath, const char * fragmentSourcePath)
{
	Graphics * graphics = Graphics::Instance();
	Shader * shader = graphics->CreateShader(vertexSourcePath,fragmentSourcePath);
	NULL_CHECK(shader,"EngineObjectManager::CreateShader -> could not create new Shader object.",NULL);
	shader->SetObjectID(GetNextObjectID());
	return shader;
}

void EngineObjectManager::DestroyShader(Shader * shader)
{
	Graphics * graphics = Graphics::Instance();
	NULL_CHECK_RTRN(shader,"EngineObjectManager::DestroyShader -> shader is NULL.");
	graphics->DestroyShader(shader);
}

Texture * EngineObjectManager::CreateTexture(const char * sourcePath, TextureAttributes attributes)
{
	Graphics * graphics = Graphics::Instance();
	Texture * texture = graphics->CreateTexture(sourcePath, attributes);
	NULL_CHECK(texture,"EngineObjectManager::CreateTexture(const char *, TextureAttributes) -> could create new Texture object.",NULL);
	texture->SetObjectID(GetNextObjectID());
	return texture;
}

Texture * EngineObjectManager::CreateTexture(const RawImage * imageData, const char * sourcePath, TextureAttributes attributes)
{
	Graphics * graphics = Graphics::Instance();
	Texture * texture = graphics->CreateTexture(imageData, sourcePath, attributes);
	NULL_CHECK(texture,"EngineObjectManager::CreateTexture(const RawImage*, const char *, TextureAttributes) -> could create new Texture object.",NULL);
	texture->SetObjectID(GetNextObjectID());
	return texture;
}

Material * EngineObjectManager::CreateMaterial(const char *name, Shader * shader)
{
	Material * m = new Material(name);
	bool initSuccess = m->Init(shader);
	if(!initSuccess)
	{
		Debug::PrintError("EngineObjectManager::CreateMaterial(Shader *) -> could not Init material");
		delete m;
		return NULL;
	}
	m->SetObjectID(GetNextObjectID());
	return m;
}

Material * EngineObjectManager::CreateMaterial(const char *name, const char * shaderVertexSourcePath, const char * shaderFragmentSourcePath)
{
	Shader * shader = CreateShader(shaderVertexSourcePath, shaderFragmentSourcePath);
	if(shader == NULL)return NULL;

	Material * m = new Material(name);
	bool initSuccess = m->Init(shader);

	if(!initSuccess)
	{
		Debug::PrintError("EngineObjectManager::CreateMaterial(const char *, const char *) -> could not Init material");
		delete m;
		return NULL;
	}
	m->SetObjectID(GetNextObjectID());
	return m;
}

void EngineObjectManager::DestroyMaterial(Material * material)
{
	NULL_CHECK_RTRN(material,"EngineObjectManager::DestroyMaterial -> material is NULL.");
	Shader * shader = material->GetShader();

	NULL_CHECK_RTRN(shader,"EngineObjectManager::DestroyMaterial -> shader is NULL.");
	DestroyShader(shader);

	delete material;
}

CameraRef EngineObjectManager::CreateCamera()
{
	Graphics * graphics = Graphics::Instance();
	Camera * camera = new Camera(graphics);
	NULL_CHECK(camera, "EngineObjectManager::CreateCamera -> Could not create new Camera object.", CameraRef::Null());
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
	NULL_CHECK_RTRN(camera, "EngineObjectManager::DeleteCamera -> camera is NULL.");
	delete camera;
}

LightRef EngineObjectManager::CreateLight()
{
	Light * light = new Light();
	NULL_CHECK(light, "EngineObjectManager::CreateLight -> Could not create new Light object.", LightRef::Null());
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
	NULL_CHECK_RTRN(light, "EngineObjectManager::DeleteLight -> light is NULL.");
	delete light;
}

