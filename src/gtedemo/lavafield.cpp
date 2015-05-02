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

/*
 * Initialize the lava field mesh and material, the textures used to render it, and load the
 * images that hold its displacement values.
 */
bool LavaField::InitMeshAndMaterial()
{
	// load first displacement image
	displacementA = ImageLoader::LoadImage("resources/textures/lava/displacementA.png");
	ASSERT(displacementA != NULL, "LavaField::InitMeshAndMaterial -> Unable to load displacement texture A.");
	// load second displacement image
	displacementB = ImageLoader::LoadImage("resources/textures/lava/displacementB.png");
	ASSERT(displacementB != NULL, "LavaField::InitMeshAndMaterial -> Unable to load displacement texture B.");

	// make lava field 1x1 in model space
	fieldWidth = 1;
	fieldHeight = 1;

	// create the lava field mesh
	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture1);
	fieldMesh = EngineUtility::CreateRectangularMesh(meshAttributes, fieldWidth, fieldHeight, subDivisions-1, subDivisions-1, false, false);
	ASSERT(fieldMesh.IsValid(), "LavaField::InitMeshAndMaterial -> Could not create lava field mesh.");

	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
	AssetImporter importer;

	TextureAttributes texAttributes;
	texAttributes.FilterMode = TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;

	// create the first texture
	lavaTextureA = objectManager->CreateTexture("resources/textures/lava/lavatex.jpg", texAttributes);
	ASSERT(lavaTextureA.IsValid(), "LavaField::InitMeshAndMaterial -> Could not create lava texture A.");
	// create the second texture
	lavaTextureB = objectManager->CreateTexture("resources/textures/lava/lavatex2.jpg", texAttributes);
	ASSERT(lavaTextureB.IsValid(), "LavaField::InitMeshAndMaterial -> Could not create lava texture B.");

	// load the lava shader and use it to create the material for rendering
	// the lava field mesh
	ShaderSource selfLitShaderSource;
	importer.LoadBuiltInShaderSource("lava", selfLitShaderSource);
	lavaMaterial = objectManager->CreateMaterial("SelfLitTexture", selfLitShaderSource);
	ASSERT(lavaMaterial.IsValid(), "LavaField::InitMeshAndMaterial -> Could not create lava material.");

	// assign the textures to the material
	lavaMaterial->SetTexture(lavaTextureA, "TEXTUREA");
	lavaMaterial->SetTexture(lavaTextureB, "TEXTUREB");
	lavaMaterial->SetSelfLit(true);

	return true;
}

/*
 * Displace the vertices in the lava field mesh to simulate flow movement with waves.
 */
void LavaField::DisplaceField()
{
	// get base pointer to displacement data in [displacementA]
	BYTE * pixelsA = displacementA->GetPixels();
	BYTE * curPixelsA;

	// get base pointer to displacement data in [displacementB]
	BYTE * pixelsB = displacementB->GetPixels();
	BYTE * curPixelsB;

	SubMesh3DRef subMesh = fieldMesh->GetSubMesh(0);
	Point3Array * positions = subMesh->GetPostions();

	// loop through each vertex in the lava field mesh and displace each
	for(unsigned int i = 0; i < positions->GetCount(); i++)
	{
		Point3 * p = positions->GetPoint(i);

		// calculate the x & y position (in model space) of the position in
		// the displacements maps from which to read displacement data
		float x = (p->x * dispTiling) + dispOffset;
		float y = (p->y * dispTiling) + dispOffset;

		// scale displacement position ranges down to [0..1].
		float percentageX = (float)x / fieldWidth;
		float percentageY = (float)y / fieldHeight;

		// calculate pixel position in displacement map images
		int pixelX = percentageX * diplacementImageDimensionSize;
		int pixelY = percentageY * diplacementImageDimensionSize;

		// enforce wrapping if pixel position is outside image boundaries
		if(pixelX > 0 && (unsigned int)pixelX >= diplacementImageDimensionSize)pixelX = pixelX % diplacementImageDimensionSize;
		if(pixelY > 0 && (unsigned int)pixelY >= diplacementImageDimensionSize)pixelY = pixelY % diplacementImageDimensionSize;
		if(pixelX < 0)pixelX = diplacementImageDimensionSize - (pixelX % diplacementImageDimensionSize);
		if(pixelY < 0)pixelY = diplacementImageDimensionSize - (pixelY % diplacementImageDimensionSize);

		// read pixel values from both displacement maps
		curPixelsA = pixelsA + ((pixelY * diplacementImageDimensionSize + pixelX) * 4); // 4 bytes per pixel
		curPixelsB = pixelsB + ((pixelY * diplacementImageDimensionSize + pixelX) * 4); // 4 bytes per pixel
		BYTE r = *curPixelsA;
		float dispA = (float)r / (float)255;
		r = *curPixelsB;
		float dispB = (float)r / (float)255;

		// calculate final displacement
		float disp = (dispA + dispB) * dispHeight - dispHeight;

		// lerp to new displacement for smooth motion
		float lerpDisp = GTEMath::Lerp(p->z, disp, .1);

		// apply displacement to Z-coordinate since mesh was aligned to XY-plane in model space.
		p->Set(p->x,p->y,lerpDisp);

	}

	// signal that mesh positions  have been modified
	subMesh->QuickUpdate();
}

/*
 * Default constructor, initialize all member variables.
 */
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

/*
 * Clean-up
 */
LavaField::~LavaField()
{

}

/*
 * Initialize the lava field. Create all scene components and objects.
 */
bool LavaField::Init()
{
	// initialize mesh, materials, textures, and displacement data
	bool baseInitSuccess = InitMeshAndMaterial();
	ASSERT(baseInitSuccess == true, "LavaField::Init -> Could not initialize lava material or lava field mesh.");

	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	// create SceneObject
	lavaFieldObject = objectManager->CreateSceneObject();
	ASSERT(lavaFieldObject.IsValid(), "LavaField::Init -> Could not create lava field scene object.");

	// set up mesh filter
	Mesh3DFilterRef meshFilter = objectManager->CreateMesh3DFilter();
	ASSERT(meshFilter.IsValid(), "LavaField::Init -> Could not create lava field mesh filter.");
	meshFilter->SetMesh3D(fieldMesh);
	meshFilter->SetCastShadows(false);
	meshFilter->SetReceiveShadows(false);

	// set up renderer
	Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	ASSERT(renderer.IsValid(), "LavaField::Init -> Could not create lava field renderer.");
	renderer->AddMaterial(lavaMaterial);

	lavaFieldObject->SetMesh3DFilter(meshFilter);
	lavaFieldObject->SetMesh3DRenderer(renderer);
	lavaFieldObject->SetActive(true);
	lavaFieldObject->SetStatic(false);
	lavaFieldObject->GetTransform().Rotate(1,0,0,-90, false);

	return true;
}

/*
 * Get the SceneObject instance that holds the lava field mesh.
 */
SceneObjectRef LavaField::GetSceneObject()
{
	return lavaFieldObject;
}

/*
 * Set the speed at which the displacement data is shifted through
 * the lava field.
 */
void LavaField::SetDisplacementSpeed(float speed)
{
	dispSpeed = speed;
}

/*
 * Set the factor by which displacement data from the image source is
 * mapped to the lava field mesh.
 */
void LavaField::SetDisplacementTileSize(float size)
{
	dispTiling = size;
}

/*
 * Set the speed at which [lavaTextureA] is animated.
 */
void LavaField::SetTextureASpeed(float speed)
{
	textureASpeed = speed;
}

/*
 * Set the speed at which [lavaTextureB] is animated.
 */
void LavaField::SetTextureBSpeed(float speed)
{
	textureBSpeed = speed;
}

/*
 * Set the height factor of the lava field displacement.
 */
void LavaField::SetDisplacementHeight(float height)
{
	dispHeight = height;
}

/*
 * The Update() method triggers the displacement of the mesh and the animation
 * of the mesh's textures.
 */
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
