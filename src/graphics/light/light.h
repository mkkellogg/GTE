#ifndef _LIGHT_H_
#define _LIGHT_H_

//forward declarations

#include "object/sceneobjectcomponent.h"
#include "graphics/color/color4.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"

enum class LightType
{
	Directional,
	Point,
	Spot
};

class Light : public SceneObjectComponent
{
	friend class EngineObjectManager;

	Color4 color;
	Vector3 direction;
	LightType type;
	float intensity;
	float attenuation;

	protected:

	Light();
	virtual ~Light();

	public:

	Color4 GetColor();
	const Color4 * GetColorPtr();
	void SetColor(Color4 color);
	void SetColor(float r, float g, float b, float a);

	Vector3 GetDirection();
	const Vector3 * GetDirectionPtr();
	void SetDirection(Vector3  direction);
	void SetDirection(float x, float y, float z);

	LightType GetType();
	void SetType(LightType type);

	void SetIntensity(float intensity);
	float GetIntensity();

	void SetAttenuation(float attenuation);
	float GetAttenuation();
};

#endif
