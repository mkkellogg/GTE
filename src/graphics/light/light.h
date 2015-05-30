#ifndef _GTE_LIGHT_H_
#define _GTE_LIGHT_H_

#include "object/sceneobjectcomponent.h"
#include "graphics/color/color4.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "base/intmask.h"
#include <memory>

namespace GTE
{
	//forward declarations
	class Light;

	enum class LightType
	{
		Directional = 1,
		Point = 2,
		Spot = 3,
		Ambient = 4
	};

	enum class LightCullType
	{
		None,
		SphereOfInfluence,
		Tiled
	};

	class Light : public SceneObjectComponent
	{
		// Since this ultimately derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;

		Color4 color;
		Vector3 direction;
		LightType type;
		float intensity;
		float attenuation;
		float range;
		bool shadowsEnabled;
		IntMask cullingMask;

	protected:

		void CalcAttentuationForCurrentRange();

		Light();
		virtual ~Light();

	public:

		Color4 GetColor() const;
		const Color4 * GetColorPtr() const;
		void SetColor(Color4 color);
		void SetColor(float r, float g, float b, float a);

		Vector3 GetDirection() const;
		const Vector3 * GetDirectionPtr() const;
		void SetDirection(Vector3  direction);
		void SetDirection(float x, float y, float z);

		LightType GetType() const;
		void SetType(LightType type);

		float GetRange() const;
		void SetRange(float range);

		void SetIntensity(float intensity);
		float GetIntensity() const;

		void SetAttenuation(float attenuation);
		float GetAttenuation() const;

		void SetShadowsEnabled(bool enabled);
		bool GetShadowsEnabled() const;

		void SetCullingMask(IntMask mask);
		IntMask GetCullingMask() const;
	};
}

#endif
