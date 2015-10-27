#ifndef _GTE_LIGHT_H_
#define _GTE_LIGHT_H_

#include "object/enginetypes.h"
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
		Ambient = 4,
		Planar = 5
	};

	enum class LightCullType
	{
		None,
		SphereOfInfluence,
		Tiled
	};

	enum class AngleAttenuationType
	{
		None = 0,
		Standard = 1,
		Past90 = 2,
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
		Real intensity;
		Bool attenuationOverride;
		Real attenuation;
		Real range;
		AngleAttenuationType parallelAttenuation;
		AngleAttenuationType orthoAttenuation;
		Bool shadowsEnabled;
		IntMask cullingMask;

	protected:

		void CalcAttentuationForCurrentRange();

		Light();
		virtual ~Light();

	public:

		Color4 GetColor() const;
		const Color4 * GetColorPtr() const;
		void SetColor(Color4 color);
		void SetColor(Real r, Real g, Real b, Real a);

		Vector3 GetDirection() const;
		const Vector3 * GetDirectionPtr() const;
		void SetDirection(Vector3  direction);
		void SetDirection(Real x, Real y, Real z);

		LightType GetType() const;
		void SetType(LightType type);

		Real GetRange() const;
		void SetRange(Real range);

		void SetIntensity(Real intensity);
		Real GetIntensity() const;

		void SetAttenuation(Real attenuation);
		Real GetAttenuation() const;
		AngleAttenuationType GetParallelAngleAttenuationType() const;
		void SetParallelAngleAttenuationType(AngleAttenuationType type);
		AngleAttenuationType GetOrthoAngleAttenuationType() const;
		void SetOrthoAngleAttenuationType(AngleAttenuationType type);

		void SetShadowsEnabled(Bool enabled);
		Bool GetShadowsEnabled() const;

		void SetCullingMask(IntMask mask);
		IntMask GetCullingMask() const;
	};
}

#endif
