/*
* class: ParticleSystem
*
* author: Mark Kellogg
*
* Core container class for a particle system.
*
*/

#ifndef _GTE_PARTICLE_SYSTEM_H_
#define _GTE_PARTICLE_SYSTEM_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "object/sceneobjectcomponent.h"
#include "particlemodifier.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/vector/vector2.h"
#include "geometry/matrix4x4.h"
#include "geometry/quaternion.h"
#include "graphics/color/color4.h"
#include "particlemodifier.h"
#include "graphics/materialvardirectory.h"
#include <vector>
#include <string>

namespace GTE
{
	// forward declarations
	class Camera;

	class ParticleSystem : public SceneObjectComponent
	{
		friend class Graphics;
		
		private:
	
		SceneObjectSharedPtr meshObject;
		Mesh3DSharedPtr mesh;
		MaterialSharedPtr particleMaterial;
		AtlasSharedPtr atlas;
		Camera* currentCamera;

		AttributeID attributeSizeID;
		AttributeID attributeRotationID;
		AttributeID attributeIndexID;

		Bool zSort;
		Bool simulateInLocalSpace;

		Bool releaseAtOnce;
		UInt32 releaseAtOnceCount = 0;
		Bool hasInitialReleaseOccurred;
		Bool isActive;

		ParticleModifier<UInt32>* atlasModifier;
		ParticleModifier<Color4>* colorModifier;
		ParticleModifier<Real>* alphaModifier;
		ParticleModifier<Vector2>* sizeModifier;

		// Particle position and position modifiers (velocity and acceleration)
		ParticleModifier<Point3>* positionModifier;
		ParticleModifier<Vector3>* velocityModifier;
		ParticleModifier<Vector3>* accelerationModifier;

		// Particle rotation and rotation modifiers (rotational speed and rotational acceleration)
		ParticleModifier<Real>* rotationModifier;
		ParticleModifier<Real>* rotationalSpeedModifier;
		ParticleModifier<Real>* rotationalAccelerationModifier;

		UInt32 particleReleaseRate;
		Real particleLifeSpan;
		Real averageParticleLifeSpan;

		UInt32 vertexCount;
		UInt32 maxParticleCount;

		UInt32 liveParticleCount;
		UInt32 deadParticleCount;
		Particle** liveParticleArray;
		Particle** deadParticleArray;

		Particle** _tempParticleArray;

		Real timeSinceLastEmit;
		Bool emitting;
		Real age;
		Real lifespan;

		// temporary storage 
		Vector3 _tempVector3;
		Quaternion _tempQuaternion;
		Matrix4x4 _tempMatrix4;

		protected:

		void CalculateAverageParticleLifeSpan();
		void CalculateMaxParticleCount();
		void GetCameraWorldAxes(Camera& camera, Vector3& axisX, Vector3 axisY, Vector3& axisZ);
		void GenerateXYAlignedQuadForParticle(Particle* particle, Vector3& axisX, Vector3 axisY, Vector3& axisZ, Point3& p1, Point3& p2, Point3& p3, Point3& p4);
		void UpdateShaderWithParticleData();
		void OnWillRender() override;

		Bool Initialize(MaterialRef material, AtlasRef atlas, Real releaseRate, Real particleLifeSpan, Real systemLifeSpan);

		Bool InitializeMesh();
		void DestroyMesh();

		void DestroyParticleArray();
		Bool InitializeParticleArray();
		Bool InitializeParticle(Particle * particle);

		void ResetParticle(Particle * particle);
		void ResetParticleDisplayAttributes(Particle * particle);
		void ResetParticlePositionData(Particle * particle);
		void ResetParticleRotationData(Particle * particle);

		void AdvanceParticle(Particle* particle, Real deltaTime);
		void AdvanceParticleDisplayAttributes(Particle * particle);
		void AdvanceParticlePositionData(Particle * particle);
		void AdvanceParticleRotationData(Particle * particle);
		void AdvanceParticles(Real deltaTime);

		void KillParticle(Particle * particle);
		void ActivateParticle(Particle* particle);
		void CleanupDeadParticles();

		void SortParticleArray(const Matrix4x4& mvpMatrix);
		void QuickSortParticleArray(Particle** array, UInt32 left, UInt32 right);

		void BindAtlasModifier(ParticleModifier<UInt32> * modifier);
		void BindColorModifier(ParticleModifier<Color4> * modifier);
		void BindAlphaModifier(ParticleModifier<Real> * modifier);
		void BindSizeModifier(ParticleModifier<Vector2> * modifier);
		void BindPositionModifier(ParticleModifier<Point3> * modifier);
		void BindVelocityModifier(ParticleModifier<Vector3> * modifier);
		void BindAccelerationModifier(ParticleModifier<Vector3> * modifier);
		void BindRotationModifier(ParticleModifier<Real> * modifier);
		void BindRotationalSpeedModifier(ParticleModifier<Real> * modifier);
		void BindRotationalAccelerationModifier(ParticleModifier<Real> * modifier);

		public:

		ParticleSystem();
		virtual ~ParticleSystem();
		void Destroy();

		static MaterialSharedPtr CreateMaterial(const std::string& shaderName, const std::string& materialName);
	};
}

#endif
