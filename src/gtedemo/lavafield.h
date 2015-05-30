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
	unsigned int subDivisions;
	// speed at which mesh displacement (lava motion) moves
	float dispSpeed;
	// speed at which lavaTextureA moves
	float textureASpeed;
	// speed at which lavaTextureB moves
	float textureBSpeed;
	// height factor for lava field displacement
	float dispHeight;
	// size of lava displacement tiles, determines how pixels in lava displacement maps
	// are mapped to the sub-sections of the lava field mesh.
	float dispTiling;

	// this value is updated based on [dispSpeed] every frame, used to find position in
	// [displacementA] and [displacementB].
	float dispOffset;
	// current offset/position of [lavaTextureA].
	float textAOffset;
	// current offset/position of [lavaTextureB].
	float textBOffset;
	// SceneObject instance that holds the lava field mesh
	GTE::SceneObjectRef lavaFieldObject;

	// width/height of both displacementA & displacementB
	const unsigned int diplacementImageDimensionSize=128;
	// width of the lava field mesh in model space
	float fieldWidth;
	// height of the lava field mesh in model space
	float fieldHeight;

	// material used to render the lava field mesh
	GTE::MaterialRef lavaMaterial;
	// the lava field mesh
	GTE::Mesh3DRef fieldMesh;
	// both textures are combined in the shader for the lava field mesh
	GTE::TextureRef lavaTextureA;
	GTE::TextureRef lavaTextureB;
	// first displacement map used is displacing the lava field mesh
	GTE::RawImage * displacementA;
	// second displacement map used is displacing the lava field mesh
	GTE::RawImage * displacementB;

	bool InitMeshAndMaterial();
	void DisplaceField();

	public:

	LavaField(unsigned int subDivisions);
	~LavaField();

	bool Init();
	GTE::SceneObjectRef GetSceneObject();
	void Update();

	void SetDisplacementSpeed(float speed);
	void SetDisplacementTileSize(float size);
	void SetTextureASpeed(float speed);
	void SetTextureBSpeed(float speed);
	void SetDisplacementHeight(float height);
};


#endif
