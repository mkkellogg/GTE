#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engineobjectmanager.h"
#include "sceneobject.h"
#include "graphics/graphics.h"
#include "graphics/shader/shader.h"
#include "graphics/light/light.h"
#include "graphics/render/material.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/stdattributes.h"
#include "graphics/object/mesh3D.h"
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
}

EngineObjectManager::~EngineObjectManager()
{

}

unsigned long EngineObjectManager::GetNextObjectID()
{
	return ++currentEngineObjectID;
}

SceneObject * EngineObjectManager::CreateSceneObject()
{
	SceneObject *obj = new SceneObject();
	NULL_CHECK(obj,"EngineObjectManager::CreateSceneObject -> could not allocate new scene object.",NULL);
	obj->SetObjectID(GetNextObjectID());
	sceneRoot.AddChild(obj);
	return obj;
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
	loadedShaders.AddShader(shaderProperties,shader);

	return true;
}

Shader *  EngineObjectManager::GetLoadedShader(LongMask properties)
{
	return loadedShaders.GetShader(properties);
}

Mesh3D * EngineObjectManager::CreateMesh3D(StandardAttributeSet attributes)
{
	Mesh3D * mesh = new Mesh3D(attributes);
	NULL_CHECK(mesh,"EngineObjectManager::CreateMesh3D -> could create new Mesh3D object.",NULL);
	mesh->SetObjectID(GetNextObjectID());
	return mesh;
}

void EngineObjectManager::DestroyMesh3D(Mesh3D * mesh)
{
	delete mesh;
}

Mesh3DRenderer * EngineObjectManager::CreateMesh3DRenderer()
{
	Graphics * graphics = Graphics::Instance();
	Mesh3DRenderer * renderer = graphics->CreateMeshRenderer();
	NULL_CHECK(renderer,"EngineObjectManager::CreateMesh3DRenderer -> could create new Mesh3DRenderer object.",NULL);
	renderer->SetObjectID(GetNextObjectID());
	return renderer;
}

void EngineObjectManager::DestroyMesh3DRenderer(Mesh3DRenderer * renderer)
{
	Graphics * graphics = Graphics::Instance();
	graphics->DestroyMeshRenderer(renderer);
}

Shader * EngineObjectManager::CreateShader(const char * vertexSourcePath, const char * fragmentSourcePath)
{
	Graphics * graphics = Graphics::Instance();
	Shader * shader = graphics->CreateShader(vertexSourcePath,fragmentSourcePath);
	NULL_CHECK(shader,"EngineObjectManager::CreateShader -> could create new Shader object.",NULL);
	shader->SetObjectID(GetNextObjectID());
	return shader;
}

void EngineObjectManager::DestroyShader(Shader * shader)
{
	Graphics * graphics = Graphics::Instance();
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
	Shader * shader = material->GetShader();
	if(shader != NULL)
	{
		DestroyShader(shader);
	}

	delete material;
}

Camera * EngineObjectManager::CreateCamera()
{
	Graphics * graphics = Graphics::Instance();
	Camera * camera = new Camera(graphics);
	NULL_CHECK(camera, "EngineObjectManager::CreateCamera -> Could not create new Camera object.", NULL);
	camera->SetObjectID(GetNextObjectID());
	return camera;
}

void EngineObjectManager::DestroyCamera(Camera * camera)
{
	delete camera;
}

Light * EngineObjectManager::CreateLight()
{
	Light * light = new Light();
	NULL_CHECK(light, "EngineObjectManager::CreateLight -> Could not create new Light object.", NULL);
	light->SetObjectID(GetNextObjectID());
	return light;
}

void EngineObjectManager::DestroyLight(Light * light)
{
	delete light;
}

const SceneObject * EngineObjectManager::GetSceneRoot() const
{
	return &sceneRoot;
}
