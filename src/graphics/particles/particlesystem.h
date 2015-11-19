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
		// Since this ultimately derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager
		friend class EngineObjectManager;

		// necessary so ParticleMeshRenderer can forward WillRender event
		friend class ParticleMeshRenderer;
		
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

		Real particleReleaseRate;
		Real particleLifeSpan;
		Real averageParticleLifeSpan;

		UInt32 vertexCount;
		UInt32 maxParticleCount;

		UInt32 liveParticleCount;
		UInt32 deadParticleCount;
		Particle** liveParticleArray;
		Particle** deadParticleArray;

		Real timeSinceLastEmit;
		Bool emitting;
		Real age;
		Real systemLifeSpan;

		UInt32 renderCount;

		Bool premultiplyAlpha;

		// temporary storage 
		Real lastDeltaTime;
		Particle** _tempParticleArray;
		Vector3 _tempVector3;
		Quaternion _tempQuaternion;
		Matrix4x4 _tempMatrix4;

		protected:

		void CalculateAverageParticleLifeSpan();
		void CalculateMaxParticleCount();
		void GetCameraWorldAxes(Camera& camera, Vector3& axisX, Vector3 axisY, Vector3& axisZ);
		void GenerateXYAlignedQuadForParticle(Particle* particle, Vector3& axisX, Vector3 axisY, Vector3& axisZ, Point3& p1, Point3& p2, Point3& p3, Point3& p4);
		void UpdateShaderWithParticleData();

		void Awake() override;
		void Start() override;
		void WillRender() override;
		void Update() override;

		Bool Initialize(MaterialRef material, AtlasRef atlas, Bool zSort, Real releaseRate, Real particleLifeSpan, Real systemLifeSpan);

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
		void AdvanceParticleDisplayAttributes(Particle * particle, Real deltaTime);
		void AdvanceParticlePositionData(Particle * particle, Real deltaTime);
		void AdvanceParticleRotationData(Particle * particle, Real deltaTime);
		void AdvanceParticles(Real deltaTime);

		void KillParticle(Particle * particle);
		void ActivateParticle(Particle* particle);
		void ActivateParticles(UInt32 count);
		void CleanupDeadParticles();

		void SortParticleArray(const Matrix4x4& mvpMatrix);
		void QuickSortParticleArray(Particle** array, Int32 left, Int32 right);
		void QuickSortSwap(Particle** particleArray, Int32 a, Int32 b);
		UInt32 QuickSortPartition(Particle** particleArray, Int32 left, Int32 right);

		ParticleSystem();
		virtual ~ParticleSystem();
		void Destroy();
		void DestroyModifiers();

		public:

		Bool BindPositionModifier(const ParticleModifier<Point3>& modifier);
		Bool BindVelocityModifier(const ParticleModifier<Vector3>& modifier);
		Bool BindAccelerationModifier(const ParticleModifier<Vector3>& modifier);
		
		Bool BindRotationModifier(const ParticleModifier<Real>& modifier);
		Bool BindRotationalSpeedModifier(const ParticleModifier<Real>& modifier);
		Bool BindRotationalAccelerationModifier(const ParticleModifier<Real>& modifier);

		Bool BindAtlasModifier(const ParticleModifier<UInt32>& modifier);
		Bool BindColorModifier(const ParticleModifier<Color4>& modifier);
		Bool BindAlphaModifier(const ParticleModifier<Real>& modifier);
		Bool BindSizeModifier(const ParticleModifier<Vector2>& modifier);

		void SetPremultiplyAlpha(Bool premultiply);
		void SetZSort(Bool sort);

		SceneObjectRef GetMeshSceneObject();

		static MaterialSharedPtr CreateMaterial(const std::string& shaderName, const std::string& materialName);
	};
}

#endif
