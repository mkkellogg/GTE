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

#include "engine.h"
#include "object/engineobject.h"
#include "scene/sceneobjectcomponent.h"
#include "particlemodifier.h"
#include "custommodifier.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/vector/vector2.h"
#include "geometry/matrix4x4.h"
#include "geometry/quaternion.h"
#include "graphics/color/color4.h"
#include "particlemodifier.h"
#include "graphics/materialvardirectory.h"

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

		public:

		enum class ModifierType
		{
			Initializer = 0,
			Updater = 1,
			All = 2
		};
		
		private:

		static const CustomModifier<UInt32> DefaultUInt32Initializer;
		static const CustomModifier<Real> DefaultRealInitializer;
		static const CustomModifier<Vector3> DefaultVector3Initializer;
		static const CustomModifier<Point3> DefaultPoint3Initializer;
		static const CustomModifier<Color4> DefaultColor4Initializer;
		static const CustomModifier<Vector2> DefaultVector2Initializer;

		static const CustomModifier<UInt32> DefaultAtlasUpdater;
		static const CustomModifier<Color4> DefaultColorUpdater;
		static const CustomModifier<Real> DefaultAlphaUpdater;
		static const CustomModifier<Vector2> DefaultSizeUpdater;
		static const CustomModifier<Point3> DefaultPositionUpdater;
		static const CustomModifier<Vector3> DefaultVelocityUpdater;
		static const CustomModifier<Vector3> DefaultAccelerationUpdater;
		static const CustomModifier<Real> DefaultRotationUpdater;
		static const CustomModifier<Real> DefaultRotationalSpeedUpdater;
		static const CustomModifier<Real> DefaultRotationalAccelerationUpdater;
	
		SceneObjectSharedPtr meshObject;
		Mesh3DSharedPtr mesh;
		MultiMaterialSharedPtr particleMaterial;
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

		ParticleModifier<UInt32>* atlasInitializer;
		ParticleModifier<Color4>* colorInitializer;
		ParticleModifier<Real>* alphaInitializer;
		ParticleModifier<Vector2>* sizeInitializer;
		ParticleModifier<UInt32>* atlasUpdater;
		ParticleModifier<Color4>* colorUpdater;
		ParticleModifier<Real>* alphaUpdater;
		ParticleModifier<Vector2>* sizeUpdater;

		// Particle position and position modifiers (velocity and acceleration)
		ParticleModifier<Point3>* positionInitializer;
		ParticleModifier<Vector3>* velocityInitializer;
		ParticleModifier<Vector3>* accelerationInitializer;
		ParticleModifier<Point3>* positionUpdater;
		ParticleModifier<Vector3>* velocityUpdater;
		ParticleModifier<Vector3>* accelerationUpdater;

		// Particle rotation and rotation modifiers (rotational speed and rotational acceleration)
		ParticleModifier<Real>* rotationInitializer;
		ParticleModifier<Real>* rotationalSpeedInitializer;
		ParticleModifier<Real>* rotationalAccelerationInitializer;
		ParticleModifier<Real>* rotationUpdater;
		ParticleModifier<Real>* rotationalSpeedUpdater;
		ParticleModifier<Real>* rotationalAccelerationUpdater;

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
		void GetCameraWorldAxes(Camera& camera, Vector3& axisX, Vector3& axisY, Vector3& axisZ);
		void GenerateXYAlignedQuadForParticle(Particle* particle, Vector3& axisX, Vector3& axisY, Vector3& axisZ, Point3& p1, Point3& p2, Point3& p3, Point3& p4);
		void UpdateShaderWithParticleData();

		void Awake() override;
		void Start() override;
		void WillRender() override;
		void Update() override;

		Bool Initialize(MultiMaterialRef material, AtlasRef atlas, Bool zSort, Real releaseRate, Real particleLifeSpan, Real systemLifeSpan);

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

		template <typename T> Bool BindModifierFromType(ParticleModifier<T>** localInitializer, ParticleModifier<T>** localUpdater, 
													const ParticleModifier<T>& modifier, ModifierType modifierType)
		{
			Bool success = true;

			if(modifierType == ModifierType::Initializer || modifierType == ModifierType::All)
			{
				success &= BindModifier(localInitializer, modifier);
			}

			if(modifierType == ModifierType::Updater || modifierType == ModifierType::All)
			{
				success &= BindModifier(localUpdater, modifier);
			}

			return success;
		}

		template <typename T> Bool BindModifier(ParticleModifier<T>** local, const ParticleModifier<T>& modifier)
		{
			SAFE_DELETE(*local);
			*local = modifier.Clone();
			NONFATAL_ASSERT_RTRN(*local != nullptr, "ParticleSystem::BindModifier -> Unable to clone modifier.", false, false);
			return true;
		}

		public:

		Bool BindPositionModifier(const ParticleModifier<Point3>& modifier, ModifierType modifierType);
		Bool BindVelocityModifier(const ParticleModifier<Vector3>& modifier, ModifierType modifierType);
		Bool BindAccelerationModifier(const ParticleModifier<Vector3>& modifier, ModifierType modifierType);
		
		Bool BindRotationModifier(const ParticleModifier<Real>& modifier, ModifierType modifierType);
		Bool BindRotationalSpeedModifier(const ParticleModifier<Real>& modifier, ModifierType modifierType);
		Bool BindRotationalAccelerationModifier(const ParticleModifier<Real>& modifier, ModifierType modifierType);

		Bool BindAtlasModifier(const ParticleModifier<UInt32>& modifier, ModifierType modifierType);
		Bool BindColorModifier(const ParticleModifier<Color4>& modifier, ModifierType modifierType);
		Bool BindAlphaModifier(const ParticleModifier<Real>& modifier, ModifierType modifierType);
		Bool BindSizeModifier(const ParticleModifier<Vector2>& modifier, ModifierType modifierType);

		void SetPremultiplyAlpha(Bool premultiply);
		void SetZSort(Bool sort);

		SceneObjectRef GetMeshSceneObject();

		static MaterialSharedPtr CreateMaterial(const std::string& shaderName, const std::string& materialName);
	};
}

#endif
