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
GTE::Bool LavaField::InitMeshAndMaterial()
{
	// load first displacement image
	displacementA = GTE::ImageLoader::LoadImageU("resources/textures/lava/displacementA.png");
	ASSERT(displacementA != nullptr, "LavaField::InitMeshAndMaterial -> Unable to load displacement texture A.");
	// load second displacement image
	displacementB = GTE::ImageLoader::LoadImageU("resources/textures/lava/displacementB.png");
	ASSERT(displacementB != nullptr, "LavaField::InitMeshAndMaterial -> Unable to load displacement texture B.");

	// make lava field 1x1 in model space
	fieldWidth = 1;
	fieldHeight = 1;

	// create the lava field mesh
	GTE::StandardAttributeSet meshAttributes = GTE::StandardAttributes::CreateAttributeSet();
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Position);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::UVTexture0);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::UVTexture1);
	fieldMesh = GTE::EngineUtility::CreateRectangularMesh(meshAttributes, fieldWidth, fieldHeight, subDivisions - 1, subDivisions - 1, false, false, false);
	ASSERT(fieldMesh.IsValid(), "LavaField::InitMeshAndMaterial -> Could not create lava field mesh.");

	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();
	GTE::AssetImporter importer;

	GTE::TextureAttributes texAttributes;
	texAttributes.FilterMode = GTE::TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;

	// create the first texture
	lavaTextureA = objectManager->CreateTexture("resources/textures/lava/lavatex.jpg", texAttributes);
	ASSERT(lavaTextureA.IsValid(), "LavaField::InitMeshAndMaterial -> Could not create lava texture A.");
	// create the second texture
	lavaTextureB = objectManager->CreateTexture("resources/textures/lava/lavatex2.jpg", texAttributes);
	ASSERT(lavaTextureB.IsValid(), "LavaField::InitMeshAndMaterial -> Could not create lava texture B.");

	// load the lava shader and use it to create the material for rendering
	// the lava field mesh
	GTE::ShaderSource selfLitShaderSource;
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
	GTE::Byte * pixelsA = displacementA->GetPixels();
	GTE::Byte * curPixelsA;

	// get base pointer to displacement data in [displacementB]
	GTE::Byte * pixelsB = displacementB->GetPixels();
	GTE::Byte * curPixelsB;

	GTE::SubMesh3DRef subMesh = fieldMesh->GetSubMesh(0);
	GTE::Point3Array * positions = subMesh->GetPostions();

	// loop through each vertex in the lava field mesh and displace each
	for(GTE::UInt32 i = 0; i < positions->GetCount(); i++)
	{
		GTE::Point3 * p = positions->GetPoint(i);

		// calculate the x & y position (in model space) of the position in
		// the displacements maps from which to read displacement data
		GTE::Real x = (p->x * dispTiling) + dispOffset;
		GTE::Real y = (p->y * dispTiling) + dispOffset;

		// scale displacement position ranges down to [0..1].
		GTE::Real percentageX = (GTE::Real)x / fieldWidth;
		GTE::Real percentageY = (GTE::Real)y / fieldHeight;

		// calculate pixel position in displacement map images
		GTE::Int32 pixelX = (GTE::Int32)(percentageX * (GTE::Real)diplacementImageDimensionSize);
		GTE::Int32 pixelY = (GTE::Int32)(percentageY * (GTE::Real)diplacementImageDimensionSize);

		// enforce wrapping if pixel position is outside image boundaries
		if(pixelX > 0 && (GTE::UInt32)pixelX >= diplacementImageDimensionSize)pixelX = pixelX % diplacementImageDimensionSize;
		if(pixelY > 0 && (GTE::UInt32)pixelY >= diplacementImageDimensionSize)pixelY = pixelY % diplacementImageDimensionSize;
		if(pixelX < 0)pixelX = diplacementImageDimensionSize - (pixelX % diplacementImageDimensionSize);
		if(pixelY < 0)pixelY = diplacementImageDimensionSize - (pixelY % diplacementImageDimensionSize);

		// read pixel values from both displacement maps
		curPixelsA = pixelsA + ((pixelY * diplacementImageDimensionSize + pixelX) * 4); // 4 bytes per pixel
		curPixelsB = pixelsB + ((pixelY * diplacementImageDimensionSize + pixelX) * 4); // 4 bytes per pixel
		GTE::Byte r = *curPixelsA;
		GTE::Real dispA = (GTE::Real)r / (GTE::Real)255;
		r = *curPixelsB;
		GTE::Real dispB = (GTE::Real)r / (GTE::Real)255;

		// calculate final displacement
		GTE::Real disp = (dispA + dispB) * dispHeight - dispHeight;

		// lerp to new displacement for smooth motion
		GTE::Real lerpDisp = GTE::GTEMath::Lerp(p->z, disp, .1f);

		// apply displacement to Z-coordinate since mesh was aligned to XY-plane in model space.
		p->Set(p->x,p->y,lerpDisp);

	}

	// signal that mesh positions  have been modified
	subMesh->QuickUpdate();
}

/*
 * Default constructor, initialize all member variables.
 */
LavaField::LavaField(GTE::UInt32 subDivisions)
{
	this->subDivisions = subDivisions;

	dispTiling = 4;
	dispOffset = 0;
	textAOffset = 0;
	textBOffset = 0;

	dispSpeed = 0;
	textureASpeed = 0;
	textureBSpeed = 0;

	displacementA = nullptr;
	displacementB = nullptr;

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
GTE::Bool LavaField::Init()
{
	// initialize mesh, materials, textures, and displacement data
	GTE::Bool baseInitSuccess = InitMeshAndMaterial();
	ASSERT(baseInitSuccess == true, "LavaField::Init -> Could not initialize lava material or lava field mesh.");

	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	// create SceneObject
	lavaFieldObject = objectManager->CreateSceneObject();
	ASSERT(lavaFieldObject.IsValid(), "LavaField::Init -> Could not create lava field scene object.");

	// set up mesh filter
	GTE::Mesh3DFilterRef meshFilter = objectManager->CreateMesh3DFilter();
	ASSERT(meshFilter.IsValid(), "LavaField::Init -> Could not create lava field mesh filter.");
	meshFilter->SetMesh3D(fieldMesh);
	meshFilter->SetCastShadows(false);
	meshFilter->SetReceiveShadows(false);

	// set up renderer
	GTE::Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
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
GTE::SceneObjectRef LavaField::GetSceneObject()
{
	return lavaFieldObject;
}

/*
 * Set the speed at which the displacement data is shifted through
 * the lava field.
 */
void LavaField::SetDisplacementSpeed(GTE::Real speed)
{
	dispSpeed = speed;
}

/*
 * Set the factor by which displacement data from the image source is
 * mapped to the lava field mesh.
 */
void LavaField::SetDisplacementTileSize(GTE::Real size)
{
	dispTiling = size;
}

/*
 * Set the speed at which [lavaTextureA] is animated.
 */
void LavaField::SetTextureASpeed(GTE::Real speed)
{
	textureASpeed = speed;
}

/*
 * Set the speed at which [lavaTextureB] is animated.
 */
void LavaField::SetTextureBSpeed(GTE::Real speed)
{
	textureBSpeed = speed;
}

/*
 * Set the height factor of the lava field displacement.
 */
void LavaField::SetDisplacementHeight(GTE::Real height)
{
	dispHeight = height;
}

/*
 * The Update() method triggers the displacement of the mesh and the animation
 * of the mesh's textures.
 */
void LavaField::Update()
{
	dispOffset += GTE::Time::GetDeltaTime() * dispSpeed;
	if (dispOffset >= (GTE::Real)diplacementImageDimensionSize * 2.0)dispOffset = 0;
	DisplaceField();

	textAOffset -= GTE::Time::GetDeltaTime() * textureASpeed;
	lavaMaterial->SetUniform2f(0,textAOffset, "UVTEXTURE0_OFFSET");

	textBOffset -= GTE::Time::GetDeltaTime() * textureBSpeed;
	lavaMaterial->SetUniform2f(0,textBOffset, "UVTEXTURE1_OFFSET");
}
