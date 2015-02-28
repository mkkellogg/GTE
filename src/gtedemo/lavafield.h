#ifndef _GTE_LAVAFIELD_H_

#include "object/enginetypes.h"
#include "graphics/image/rawimage.h"
#include "graphics/image/imageloader.h"

class LavaField
{
	unsigned int subDivisions;
	float dispSpeed;
	float textureASpeed;
	float textureBSpeed;
	float dispHeight;
	float dispTiling;

	float dispOffset;
	float textAOffset;
	float textBOffset;
	SceneObjectRef lavaFieldObject;

	const unsigned int diplacementImageDimensionSize=128;
	float fieldWidth;
	float fieldHeight;

	MaterialRef lavaMaterial;
	Mesh3DRef fieldMesh;
	TextureRef lavaTextureA;
	TextureRef lavaTextureB;
	RawImage * displacementA;
	RawImage * displacementB;

	bool InitMeshAndMaterial();
	void DisplaceField();

	public:

	LavaField(unsigned int subDivisions);
	~LavaField();

	bool Init();
	SceneObjectRef GetSceneObject();
	void Update();

	void SetDisplacementSpeed(float speed);
	void SetDisplacementTileSize(float size);
	void SetTextureASpeed(float speed);
	void SetTextureBSpeed(float speed);
	void SetDisplacementHeight(float height);
};


#endif
