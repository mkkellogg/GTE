/*
 * Class: LavaField
 *
 * Author: Mark Kellogg
 *
 * This class manages the creation and initialization of a lava field mesh & materials,
 * as well as animations of its textures and displacement of the mesh vertices to
 * simulate motion of the lava.
 */

#ifndef _GTE_LAVAFIELD_H_

#include "object/enginetypes.h"
#include "graphics/image/rawimage.h"
#include "graphics/image/imageloader.h"

class LavaField
{
	// the lava field mesh is divided into subDivisions x subDivisions sub-sections
	GTE::UInt32 subDivisions;
	// speed at which mesh displacement (lava motion) moves
	GTE::Real dispSpeed;
	// speed at which lavaTextureA moves
	GTE::Real textureASpeed;
	// speed at which lavaTextureB moves
	GTE::Real textureBSpeed;
	// height factor for lava field displacement
	GTE::Real dispHeight;
	// size of lava displacement tiles, determines how pixels in lava displacement maps
	// are mapped to the sub-sections of the lava field mesh.
	GTE::Real dispTiling;

	// this value is updated based on [dispSpeed] every frame, used to find position in
	// [displacementA] and [displacementB].
	GTE::Real dispOffset;
	// current offset/position of [lavaTextureA].
	GTE::Real textAOffset;
	// current offset/position of [lavaTextureB].
	GTE::Real textBOffset;
	// SceneObject instance that holds the lava field mesh
	GTE::SceneObjectSharedPtr lavaFieldObject;

	// width/height of both displacementA & displacementB
	const GTE::UInt32 diplacementImageDimensionSize = 128;
	// width of the lava field mesh in model space
	GTE::Real fieldWidth;
	// height of the lava field mesh in model space
	GTE::Real fieldHeight;

	// material used to render the lava field mesh
	GTE::MaterialSharedPtr lavaMaterial;
	// the lava field mesh
	GTE::Mesh3DSharedPtr fieldMesh;
	// both textures are combined in the shader for the lava field mesh
	GTE::TextureSharedPtr lavaTextureA;
	GTE::TextureSharedPtr lavaTextureB;
	// first displacement map used is displacing the lava field mesh
	GTE::RawImage * displacementA;
	// second displacement map used is displacing the lava field mesh
	GTE::RawImage * displacementB;

	GTE::Bool InitMeshAndMaterial();
	void DisplaceField();

public:

	LavaField(GTE::UInt32 subDivisions);
	~LavaField();

	GTE::Bool Init();
	GTE::SceneObjectSharedPtr GetSceneObject();
	void Update();

	void SetDisplacementSpeed(GTE::Real speed);
	void SetDisplacementTileSize(GTE::Real size);
	void SetTextureASpeed(GTE::Real speed);
	void SetTextureBSpeed(GTE::Real speed);
	void SetDisplacementHeight(GTE::Real height);
};


#endif
