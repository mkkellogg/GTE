#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <memory>
#include "graphics/graphics.h"
#include "lavafield.h"
#include "engine.h"
#include "asset/assetimporter.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "graphics/stdattributes.h"
#include "graphics/shader/shadersource.h"
#include "graphics/render/material.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/texture/textureattr.h"
#include "gtemath/gtemath.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "util/time.h"
#include "util/engineutility.h"


bool LavaField::InitMeshAndMaterial()
{
	displacementA = ImageLoader::LoadImage("resources/textures/lava/displacementA.png");
	ASSERT(displacementA != NULL, "LavaField::InitMeshAndMaterial -> Unable to load displacement texture A.", false);

	displacementB = ImageLoader::LoadImage("resources/textures/lava/displacementB.png");
	ASSERT(displacementB != NULL, "LavaField::InitMeshAndMaterial -> Unable to load displacement texture B.", false);

	fieldWidth = 1;
	fieldHeight = 1;
	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::FaceNormal);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture1);
	fieldMesh = EngineUtility::CreateRectangularMesh(meshAttributes, fieldWidth, fieldHeight, subDivisions-1, subDivisions-1);
	ASSERT(fieldMesh.IsValid(), "LavaField::InitMeshAndMaterial -> Could not create field mesh.", false);

	fieldMesh->SetCastShadows(false);
	fieldMesh->SetReceiveShadows(false);

	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
	AssetImporter importer;

	// load texture for the cube
	TextureAttributes texAttributes;
	texAttributes.FilterMode = TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;
	lavaTextureA = objectManager->CreateTexture("resources/textures/lava/lavatex.jpg", texAttributes);
	ASSERT(lavaTextureA.IsValid(), "LavaField::InitMeshAndMaterial -> Could not create lava texture A.", false);
	lavaTextureB = objectManager->CreateTexture("resources/textures/lava/lavatex2.jpg", texAttributes);
	ASSERT(lavaTextureB.IsValid(), "LavaField::InitMeshAndMaterial -> Could not create lava texture B.", false);

	ShaderSource selfLitShaderSource;
	importer.LoadBuiltInShaderSource("lava", selfLitShaderSource);
	lavaMaterial = objectManager->CreateMaterial("SelfLitTexture", selfLitShaderSource);
	ASSERT(lavaMaterial.IsValid(), "LavaField::InitMeshAndMaterial -> Could not create lava material.", false);

	lavaMaterial->SetTexture(lavaTextureA, "TEXTUREA");
	lavaMaterial->SetTexture(lavaTextureB, "TEXTUREB");
	lavaMaterial->SetSelfLit(true);

	return true;
}

void LavaField::DisplaceField()
{
	BYTE * pixelsA = displacementA->GetPixels();
	BYTE * curPixelsA;

	BYTE * pixelsB = displacementB->GetPixels();
	BYTE * curPixelsB;

	SubMesh3DRef subMesh = fieldMesh->GetSubMesh(0);
	Point3Array * positions = subMesh->GetPostions();

	for(unsigned int i = 0; i < positions->GetCount(); i++)
	{
		Point3 * p = positions->GetPoint(i);
		float x = (p->x * dispTiling) + dispOffset;
		float y = (p->y * dispTiling) + dispOffset;

		float percentageX = (float)x / fieldWidth;
		float percentageY = (float)y / fieldHeight;

		unsigned int pixelX = percentageX * diplacementImageDimensionSize;
		unsigned int pixelY = percentageY * diplacementImageDimensionSize;

		if(pixelX >= diplacementImageDimensionSize)pixelX = pixelX % diplacementImageDimensionSize;
		if(pixelY >= diplacementImageDimensionSize)pixelY = pixelY % diplacementImageDimensionSize;

		if(pixelX < 0)pixelX = diplacementImageDimensionSize - (pixelX % diplacementImageDimensionSize);
		if(pixelY < 0)pixelY = diplacementImageDimensionSize - (pixelY % diplacementImageDimensionSize);

		curPixelsA = pixelsA + ((pixelY * diplacementImageDimensionSize + pixelX) * 4);
		curPixelsB = pixelsB + ((pixelY * diplacementImageDimensionSize + pixelX) * 4);

		BYTE r = *curPixelsA;
		float dispA = (float)r / (float)255;

		r = *curPixelsB;
		float dispB = (float)r / (float)255;

		float disp = (dispA + dispB) * dispHeight - dispHeight;

		float lerpDisp = GTEMath::Lerp(p->z, disp, .1);
		p->Set(p->x,p->y,lerpDisp);

	}

	subMesh->QuickUpdate();
}

LavaField::LavaField(unsigned int subDivisions)
{
	this->subDivisions = subDivisions;

	dispTiling = 4;
	dispOffset = 0;
	textAOffset = 0;
	textBOffset = 0;

	dispSpeed = 0;
	textureASpeed = 0;
	textureBSpeed = 0;

	displacementA = NULL;
	displacementB = NULL;

	fieldWidth = 0;
	fieldHeight = 0;
	dispHeight =5;
}

LavaField::~LavaField()
{

}

bool LavaField::Init()
{
	bool baseInitSuccess = InitMeshAndMaterial();
	ASSERT(baseInitSuccess == true, "LavaField::Init -> Could not initialize lava material or lava field mesh.", false);

	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	lavaFieldObject = objectManager->CreateSceneObject();
	ASSERT(lavaFieldObject.IsValid(), "LavaField::Init -> Could not create lava field scene object.", false);

	Mesh3DFilterRef meshFilter = objectManager->CreateMesh3DFilter();
	ASSERT(meshFilter.IsValid(), "LavaField::Init -> Could not create lava field mesh filter.", false);

	Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	ASSERT(renderer.IsValid(), "LavaField::Init -> Could not create lava field renderer.", false);

	meshFilter->SetMesh3D(fieldMesh);
	lavaFieldObject->SetMesh3DFilter(meshFilter);

	renderer->AddMaterial(lavaMaterial);
	lavaFieldObject->SetMesh3DRenderer(renderer);

	lavaFieldObject->SetActive(true);
	lavaFieldObject->SetStatic(false);
	lavaFieldObject->GetTransform().Rotate(1,0,0,-90, false);

	return true;
}

SceneObjectRef LavaField::GetSceneObject()
{
	return lavaFieldObject;
}

void LavaField::SetDisplacementSpeed(float speed)
{
	dispSpeed = speed;
}

void LavaField::SetDisplacementTileSize(float size)
{
	dispTiling = size;
}

void LavaField::SetTextureASpeed(float speed)
{
	textureASpeed = speed;
}

void LavaField::SetTextureBSpeed(float speed)
{
	textureBSpeed = speed;
}

void LavaField::SetDisplacementHeight(float height)
{
	dispHeight = height;
}

void LavaField::Update()
{
	dispOffset += Time::GetDeltaTime() * dispSpeed;
	if(dispOffset >= (float)diplacementImageDimensionSize * 2.0)dispOffset = 0;
	DisplaceField();

	textAOffset -= Time::GetDeltaTime() * textureASpeed;
	lavaMaterial->SetUniform2f(0,textAOffset, "UVTEXTURE0_OFFSET");

	textBOffset -=  Time::GetDeltaTime() * textureBSpeed;
	lavaMaterial->SetUniform2f(0,textBOffset, "UVTEXTURE1_OFFSET");
}
