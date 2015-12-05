#include <memory.h>

#include "engine.h"
#include "particlesystem.h"
#include "particleutil.h"
#include "particle.h"
#include "particlemeshrenderer.h"
#include "framesetmodifier.h"
#include "randommodifier.h"
#include "evenintervalindexmodifier.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "scene/sceneobject.h"
#include "object/engineobjectmanager.h"
#include "asset/assetimporter.h"
#include "graphics/shader/shadersource.h"
#include "graphics/render/material.h"
#include "graphics/render/multimaterial.h"
#include "graphics/renderstate.h"
#include "graphics/stdattributes.h"
#include "graphics/object/submesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/object/customfloatattributebuffer.h"
#include "graphics/view/camera.h"
#include "graphics/texture/atlas.h"
#include "util/time.h"

namespace GTE
{
	ParticleSystem::ParticleSystem()
	{
		currentCamera = nullptr;

		zSort = false;
		simulateInLocalSpace = true;
	
		releaseAtOnce = false;
		releaseAtOnceCount = 0;
		hasInitialReleaseOccurred = false;
		isActive = false;

		attributeSizeID = AttributeDirectory::VarID_Invalid;
		attributeRotationID = AttributeDirectory::VarID_Invalid;
		attributeIndexID = AttributeDirectory::VarID_Invalid;

		atlasModifier = nullptr;
		colorModifier = nullptr;
		alphaModifier = nullptr;
		sizeModifier = nullptr;

		// Particle position and position modifiers (velocity and acceleration)
		positionModifier = nullptr;
		velocityModifier = nullptr;
		accelerationModifier = nullptr;

		// Particle rotation and rotation modifiers (rotational speed and rotational acceleration)
		rotationModifier = nullptr;
		rotationalSpeedModifier = nullptr;
		rotationalAccelerationModifier = nullptr;

		particleReleaseRate = 100;
		particleLifeSpan = 1.0f;
		averageParticleLifeSpan = 1.0f;
		CalculateAverageParticleLifeSpan();

		vertexCount = 0;
		maxParticleCount = 0;
		CalculateMaxParticleCount();
		liveParticleCount = 0;
		deadParticleCount = 0;
		liveParticleArray = nullptr;
		deadParticleArray = nullptr;
		_tempParticleArray = nullptr;

		timeSinceLastEmit = 0.0f;
		emitting = true;
		age = 0.0f;
		systemLifeSpan = 0.0f;

		renderCount = 0;

		premultiplyAlpha = false;
	}

	ParticleSystem::~ParticleSystem()
	{
		Destroy();
	}

	void ParticleSystem::Destroy()
	{
		DestroyParticleArray();
		DestroyModifiers();
	}

	void ParticleSystem::DestroyModifiers()
	{
		SAFE_DELETE(atlasModifier);
		SAFE_DELETE(colorModifier);
		SAFE_DELETE(alphaModifier);
		SAFE_DELETE(sizeModifier);

		SAFE_DELETE(positionModifier);
		SAFE_DELETE(velocityModifier);
		SAFE_DELETE(accelerationModifier);

		SAFE_DELETE(rotationModifier);
		SAFE_DELETE(rotationalSpeedModifier);
		SAFE_DELETE(rotationalAccelerationModifier);
	}

	MaterialSharedPtr ParticleSystem::CreateMaterial(const std::string& shaderName, const std::string& materialName)
	{
		GTE::AssetImporter importer;
		GTE::ShaderSource shaderSource;

		GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();
		importer.LoadBuiltInShaderSource(shaderName, shaderSource);
		MaterialRef material = objectManager->CreateMaterial(materialName, shaderSource);

		material->SetUseLighting(false);
		material->SetRenderQueue(RenderQueueType::Transparent);
		material->SetDepthBufferWriteEnabled(false);
		material->SetDepthBufferFunction(RenderState::DepthBufferFunction::LessThanOrEqual);
		material->SetBlendingMode(RenderState::BlendingMode::Additive);
		material->SetFaceCulling(RenderState::FaceCulling::None);

		return material;
	}

	void ParticleSystem::CalculateAverageParticleLifeSpan()
	{
		Real total = 0.0f;

		for(UInt32 i = 0; i < 100; i++)
		{
			total += particleLifeSpan;
		}

		total /= 100.0f;

		averageParticleLifeSpan = total;
	}

	void ParticleSystem::CalculateMaxParticleCount()
	{
		if(releaseAtOnce)
		{
			maxParticleCount = releaseAtOnceCount;
		}
		else
		{
			Real minLifeSpan = particleLifeSpan;
			if(systemLifeSpan != 0 && systemLifeSpan  < minLifeSpan) minLifeSpan = systemLifeSpan;
			maxParticleCount = (UInt32)GTEMath::Max(particleReleaseRate * minLifeSpan * 2, 1.0f);
		}

		vertexCount = maxParticleCount * (UInt32)ParticleConstants::VerticesPerParticle;
	}

	void ParticleSystem::GetCameraWorldAxes(Camera& camera, Vector3& axisX, Vector3& axisY, Vector3& axisZ)
	{
		SceneObjectRef cameraSceneObject = camera.GetSceneObject();
		Transform cameraWorld;
		SceneObjectTransform::GetWorldTransform(cameraWorld, cameraSceneObject.GetRef(), true, false);

		axisZ.Set(0, 0, 1);
		axisY.Set(0, 1, 0);

		cameraWorld.TransformVector(axisZ);
		cameraWorld.TransformVector(axisY);

		Vector3::Cross(axisY, axisZ, axisX);

		axisY.Normalize();
		axisX.Normalize();
		axisZ.Normalize();
	}

	void ParticleSystem::GenerateXYAlignedQuadForParticle(Particle* particle, Vector3& axisX, Vector3& axisY, Vector3& axisZ, Point3& p1, Point3& p2, Point3& p3, Point3& p4)
	{
		Point3 position = particle->Position;
		Real rotation = particle->Rotation;

		Vector3 vectorX = axisX;
		Vector3 vectorY = axisY;

		vectorX.Scale(GTEMath::Cos(rotation * Constants::DegreesToRads));
		vectorY.Scale(GTEMath::Sin(rotation * Constants::DegreesToRads));

		vectorX.Add(vectorY);
		Vector3::Cross(axisZ, vectorX, vectorY);

		vectorX.Scale(particle->Size.x);
		vectorY.Scale(particle->Size.y);

		Point3::Subtract(position, vectorX, p1);
		Point3::Add(p1, vectorY, p1);
		Point3::Subtract(position, vectorX, p2);
		Point3::Subtract(p2, vectorY, p2);
		Point3::Add(position, vectorX, p3);
		Point3::Subtract(p3, vectorY, p3);
		Point3::Add(position, vectorX, p3);
		Point3::Add(p3, vectorY, p3);
	}

	void ParticleSystem::UpdateShaderWithParticleData()
	{
		Vector3 vectorX;
		Vector3 vectorY;
		Vector3 vectorZ;

		NONFATAL_ASSERT(currentCamera != nullptr, "ParticleSystem::UpdateShaderWithParticleData -> No valid camera set.", true);

		GetCameraWorldAxes(*currentCamera, vectorX, vectorY, vectorZ);

		for(UInt32 i = 0; i < particleMaterial->GetMaterialCount(); i++)
		{
			MaterialRef mat = particleMaterial->GetMaterial(i);
			mat->SetUniform3f(vectorX.x, vectorX.y, vectorX.z, "VIEW_AXIS_X");
			mat->SetUniform3f(vectorY.x, vectorY.y, vectorY.z, "VIEW_AXIS_Y");
			mat->SetUniform3f(vectorZ.x, vectorZ.y, vectorZ.z, "VIEW_AXIS_Z");
			mat->SetTexture(atlas->GetTexture(), "PARTICLE_TEXTURE");
		}

		SubMesh3DRef targetMesh = mesh->GetSubMesh(0);
		NONFATAL_ASSERT(targetMesh.IsValid(), "ParticleSystem::UpdateShaderWithParticleData -> Target mesh is invalid.", true);

		Real maxX, maxY, maxZ, minX, minY, minZ;
		maxX = maxY = maxZ = minX = minY = minZ = 0;

		for(UInt32 p = 0; p < liveParticleCount; p++)
		{
			Particle* particle = liveParticleArray[p];
			Point3& position = particle->Position;
			Real rotation = particle->Rotation;

			if(position.x > maxX || p == 0)maxX = position.x;
			if(position.x < minX || p == 0)minX = position.x;
			if(position.y > maxY || p == 0)maxY = position.y;
			if(position.y < minY || p == 0)minY = position.y;
			if(position.z > maxZ || p == 0)maxZ = position.z;
			if(position.z < minZ || p == 0)minZ = position.z;

			UInt32 baseIndex = p * (UInt32)ParticleConstants::VerticesPerParticle;

			Point3Array * positions = targetMesh->GetPostions();
			positions->GetElement(baseIndex)->SetTo(position);
			positions->GetElement(baseIndex + 1)->SetTo(position);
			positions->GetElement(baseIndex + 2)->SetTo(position);
			positions->GetElement(baseIndex + 3)->SetTo(position);
			positions->GetElement(baseIndex + 4)->SetTo(position);
			positions->GetElement(baseIndex + 5)->SetTo(position);
		
			Atlas::ImageDescriptor * imageDesc = atlas->GetImageDescriptor(particle->AtlasIndex);
			UV2Array * uvs = targetMesh->GetUVs0();
			uvs->GetElement(baseIndex)->Set(imageDesc->Left, imageDesc->Top);
			uvs->GetElement(baseIndex + 1)->Set(imageDesc->Right, imageDesc->Top);
			uvs->GetElement(baseIndex + 2)->Set(imageDesc->Left, imageDesc->Bottom);
			uvs->GetElement(baseIndex + 3)->Set(imageDesc->Left, imageDesc->Bottom);
			uvs->GetElement(baseIndex + 4)->Set(imageDesc->Right, imageDesc->Top);
			uvs->GetElement(baseIndex + 5)->Set(imageDesc->Right, imageDesc->Bottom);

			Color4Array * colors = targetMesh->GetColors();
			Color4 color = particle->Color;
			color.a = particle->Alpha;
			if(premultiplyAlpha)
			{
				color.Scale(particle->Alpha);
			}
			colors->GetElement(baseIndex)->SetTo(color);
			colors->GetElement(baseIndex + 1)->SetTo(color);
			colors->GetElement(baseIndex + 2)->SetTo(color);
			colors->GetElement(baseIndex + 3)->SetTo(color);
			colors->GetElement(baseIndex + 4)->SetTo(color);
			colors->GetElement(baseIndex + 5)->SetTo(color);

			CustomFloatAttributeBuffer * sizeAttribute = targetMesh->GetCustomFloatAttributeBufferByID(attributeSizeID);
			CustomFloatAttributeBuffer * rotationAttribute = targetMesh->GetCustomFloatAttributeBufferByID(attributeRotationID);
			CustomFloatAttributeBuffer * indexAttribute = targetMesh->GetCustomFloatAttributeBufferByID(attributeIndexID);

			Real* sizeData = sizeAttribute->GetDataPtr();
			UInt32 sizeComponents = sizeAttribute->GetComponentCount();
			for(UInt32 i = 0; i < (UInt32)ParticleConstants::VerticesPerParticle; i++)
			{
				sizeData[(sizeComponents * (baseIndex + i))] = particle->Size.x;
				sizeData[(sizeComponents * (baseIndex + i)) + 1] = particle->Size.y;
			}

			Real* rotationData = rotationAttribute->GetDataPtr();
			UInt32 rotationComponents = rotationAttribute->GetComponentCount();
			for(UInt32 i = 0; i < (UInt32)ParticleConstants::VerticesPerParticle; i++)
			{
				rotationData[rotationComponents * (baseIndex + i)] = rotation * Constants::DegreesToRads;
			}

			Real* indexData = indexAttribute->GetDataPtr();
			UInt32 indexComponents = indexAttribute->GetComponentCount();
			indexData[indexComponents * (baseIndex)] = 0;
			indexData[indexComponents * (baseIndex + 1)] = 3;
			indexData[indexComponents * (baseIndex + 2)] = 1;
			indexData[indexComponents * (baseIndex + 3)] = 1;
			indexData[indexComponents * (baseIndex + 4)] = 3;
			indexData[indexComponents * (baseIndex + 5)] = 2;
		}

		// get the dimensions of the rectangular volume formed by the
		// maximum extents
		Real width = maxX - minX;
		Real height = maxY - minY;
		Real depth = maxZ - minZ;

		// calculate the mesh's center
		Point3 center;
		center.x = width / 2.0f + minX;
		center.y = height / 2.0f + minY;
		center.z = depth / 2.0f + minZ;

		Vector3 boundingBox;
		boundingBox.x = width / 2.0f;
		boundingBox.y = height / 2.0f;
		boundingBox.z = depth / 2.0f;

		targetMesh->SetBoundingBox(boundingBox);
		targetMesh->SetCenter(center);

		targetMesh->SetRenderVertexCount(liveParticleCount * (UInt32)ParticleConstants::VerticesPerParticle);
		targetMesh->QuickUpdate();
	}

	void ParticleSystem::Awake()
	{

	}

	void ParticleSystem::Start()
	{
		isActive = true;
		emitting = true;
	}

	void ParticleSystem::WillRender()
	{
		CameraRef currentCamera = Engine::Instance()->GetRenderManager()->GetCurrentCamera();
		this->currentCamera = currentCamera.GetPtr();

		SceneObjectRef cameraObject = currentCamera->GetSceneObject();
		NONFATAL_ASSERT(cameraObject.IsValid(), "ParticleSystem::WillRender -> Camera is not attached to a valid scene object.", true);

		SceneObjectRef thisSceneObject = GetSceneObject();
		NONFATAL_ASSERT(thisSceneObject.IsValid(), "ParticleSystem::WillRender -> Particle system is not attached to a valid scene object.", true);

		Transform thisWorldTransform;
		SceneObjectTransform::GetWorldTransform(thisWorldTransform, thisSceneObject, true, false);

		if(zSort)
		{
			Transform cameraTransform;
			SceneObjectTransform::GetWorldTransform(cameraTransform, cameraObject, true, false);
			cameraTransform.Invert();
			cameraTransform.TransformBy(thisWorldTransform);
			cameraTransform.PreTransformBy(currentCamera->GetProjectionTransform());
			SortParticleArray(cameraTransform.GetMatrix());
		}
		UpdateShaderWithParticleData();

		age += lastDeltaTime;
		if(systemLifeSpan != 0.0f && age > systemLifeSpan)
		{
			emitting = false;
		}

		renderCount++;
	}

	void ParticleSystem::Update()
	{
		SceneObjectRef thisSceneObject = GetSceneObject();
		NONFATAL_ASSERT(thisSceneObject.IsValid(), "ParticleSystem::Update -> Particle system is not atached to a valid scene object.", true);

		SceneObjectRef meshObjectParent = meshObject->GetParent();
		if(!meshObjectParent.IsValid() || meshObjectParent->GetObjectID() != thisSceneObject->GetObjectID())
		{
			thisSceneObject->AddChild(meshObject);
			meshObject->SetActive(true);
			meshObject->GetTransform().SetIdentity();
		}

		if(simulateInLocalSpace)
		{
			meshObject->SetInheritsTransform(true);
			meshObject->GetTransform().SetIdentity();
		}
		else
		{
			meshObject->SetInheritsTransform(false);
		}
		
		RendererRef renderer = meshObject->GetRenderer();
		if(renderer.IsValid())
		{
			Renderer* baseRenderer = renderer.GetPtr();
			ParticleMeshRenderer * meshRenderer = dynamic_cast<ParticleMeshRenderer*>(baseRenderer);

			if(meshRenderer != nullptr)
			{
				meshRenderer->SetTargetSystemObject(GetSceneObject());
			}
		}

		renderCount = 0;

		if(!emitting)return;
		if(!isActive)return;

		lastDeltaTime = Time::GetDeltaTime();
		timeSinceLastEmit += lastDeltaTime;

		if(releaseAtOnce)
		{
			Real waitTime = averageParticleLifeSpan;

			if(!hasInitialReleaseOccurred || (timeSinceLastEmit > waitTime && liveParticleCount <= 0))
			{
				ActivateParticles(maxParticleCount);
				timeSinceLastEmit = 0.0f;
				hasInitialReleaseOccurred = true;
			}
		}
		else
		{
			Real emitUnitTime = 1.0f / particleReleaseRate;
			if(!hasInitialReleaseOccurred || timeSinceLastEmit > emitUnitTime)
			{
				Int32 releaseCount = GTEMath::Max(1, (Int32)(timeSinceLastEmit / emitUnitTime));
				ActivateParticles(releaseCount);
				timeSinceLastEmit = 0.0f;
				hasInitialReleaseOccurred = true;
			}
		}

		AdvanceParticles(lastDeltaTime);
	}
	
	Bool ParticleSystem::Initialize(MultiMaterialRef material, AtlasRef atlas, Bool zSort, Real releaseRate, Real particleLifeSpan, Real systemLifeSpan)
	{
		timeSinceLastEmit = 0.0f;
		age = 0.0;
		emitting = true;

		this->particleMaterial = material;
		this->atlas = atlas;
		this->zSort = zSort;
		this->particleReleaseRate = releaseRate;
		this->particleLifeSpan = particleLifeSpan;
		this->systemLifeSpan = systemLifeSpan;

		CalculateAverageParticleLifeSpan();
		CalculateMaxParticleCount();
		Bool particleArraySuccess = InitializeParticleArray();
		Bool meshSuccess = InitializeMesh();
		return particleArraySuccess && meshSuccess;
	}

	Bool ParticleSystem::InitializeMesh()
	{
		DestroyMesh();
		
		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

		meshObject = objectManager->CreateSceneObject();
		NONFATAL_ASSERT_RTRN(meshObject.IsValid(), "ParticleSystem::InitializeMesh -> Could not create mesh object.", false, false);

		StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::FaceNormal);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::VertexColor);
		StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
		
		SubMesh3DSharedPtr subMesh = objectManager->CreateSubMesh3D(meshAttributes);
		NONFATAL_ASSERT_RTRN(subMesh.IsValid(), "ParticleSystem::InitializeMesh -> Could not create sub mesh object.", false, false);
		subMesh->SetBuildFaces(false);
		subMesh->SetCalculateTangents(false);
		subMesh->SetCalculateNormals(false);
		subMesh->Init(vertexCount);

		mesh = objectManager->CreateMesh3D(1);
		mesh->Init();
		mesh->SetSubMesh(subMesh, 0);
		mesh->UpdateAll();
		
		attributeSizeID = subMesh->AddCustomFloatAttributeBuffer(2, "PARTICLE_SIZE");
		NONFATAL_ASSERT_RTRN(attributeSizeID != AttributeDirectory::VarID_Invalid, "ParticleSystem::InitializeMesh -> Could not add custom attribute 'PARTICLE_SIZE'.", false, false);

		attributeRotationID = subMesh->AddCustomFloatAttributeBuffer(1, "PARTICLE_ROTATION");
		NONFATAL_ASSERT_RTRN(attributeRotationID != AttributeDirectory::VarID_Invalid, "ParticleSystem::InitializeMesh -> Could not add custom attribute 'PARTICLE_ROTATION'.", false, false);

		attributeIndexID = subMesh->AddCustomFloatAttributeBuffer(1, "PARTICLE_INDEX");
		NONFATAL_ASSERT_RTRN(attributeIndexID != AttributeDirectory::VarID_Invalid, "ParticleSystem::InitializeMesh -> Could not add custom attribute 'PARTICLE_INDEX'.", false, false);

		Mesh3DFilterSharedPtr filter = objectManager->CreateMesh3DFilter();
		NONFATAL_ASSERT_RTRN(filter.IsValid(), "ParticleSystem::InitializeMesh -> Could not create mesh filter.", false, false);
		meshObject->SetMesh3DFilter(filter);
		filter->SetMesh3D(mesh);
		filter->SetCastShadows(false);
		filter->SetReceiveShadows(false);

		ParticleMeshRendererSharedPtr meshRenderer = objectManager->CreateParticleMeshRenderer();
		NONFATAL_ASSERT_RTRN(meshRenderer.IsValid(), "ParticleSystem::InitializeMesh -> Could not create mesh renderer.", false, false);
		NONFATAL_ASSERT_RTRN(particleMaterial.IsValid(), "ParticleSystem::InitializeMesh -> Particle material is not valid.", false, false);
		meshRenderer->AddMultiMaterial(particleMaterial);
		meshObject->SetRenderer(GTE::DynamicCastEngineObject<GTE::ParticleMeshRenderer, GTE::Renderer>(meshRenderer));
		meshObject->SetActive(false);
		
		return true;
	}

	void ParticleSystem::DestroyMesh()
	{
		if(meshObject.IsValid())
		{
			Mesh3DFilterRef meshFilter = meshObject->GetMesh3DFilter();

			if(meshFilter.IsValid())
			{
				meshFilter->RemoveMesh3D();
				meshObject->RemoveMesh3DFilter();
				meshObject->RemoveRenderer();
			}

			if(!mesh.IsValid())return;

			mesh = Mesh3DSharedPtr::Null();
			meshObject = SceneObjectSharedPtr::Null();
		}
	}

	Bool ParticleSystem::InitializeParticleArray()
	{
		DestroyParticleArray();
		
		liveParticleArray = new(std::nothrow)Particle*[maxParticleCount];
		ASSERT(liveParticleArray != nullptr, "ParticleSystem::InitializeParticleArray -> Unable to allocate live particle array.");
	
		deadParticleArray = new(std::nothrow)Particle*[maxParticleCount];
		ASSERT(deadParticleArray != nullptr, "ParticleSystem::InitializeParticleArray -> Unable to allocate dead particle array.");

		_tempParticleArray = new(std::nothrow)Particle*[maxParticleCount];
		ASSERT(_tempParticleArray != nullptr, "ParticleSystem::InitializeParticleArray -> Unable to allocate temp particle array.");

		for (UInt32 i = 0; i < maxParticleCount; i++ )
		{
			Particle* particle = new(std::nothrow) Particle();
			ASSERT(particle != nullptr, "ParticleSystem::InitializeParticleArray -> Unable to allocate live particle array.");
			
			InitializeParticle(particle);
			deadParticleArray[i] = particle;
		}

		liveParticleCount = 0;
		deadParticleCount = maxParticleCount;

		return true;
	}

	void ParticleSystem::DestroyParticleArray()
	{
		if(liveParticleArray != nullptr &&
		   deadParticleArray != nullptr &&
		   _tempParticleArray != nullptr)
		{
			for(UInt32 i = 0; i < liveParticleCount; i++)
			{
				SAFE_DELETE(liveParticleArray[i]);
			}

			for(UInt32 i = 0; i < deadParticleCount; i++)
			{
				SAFE_DELETE(deadParticleArray[i]);
			}

			SAFE_DELETE_ARRAY(liveParticleArray);
			SAFE_DELETE_ARRAY(deadParticleArray);
			SAFE_DELETE_ARRAY(_tempParticleArray);
		}
	}

	Bool ParticleSystem::InitializeParticle(Particle * particle)
	{
		ResetParticle(particle);
		return true;
	}

	void ParticleSystem::ResetParticle(Particle * particle)
	{
		particle->Age = 0;
		particle->Alive = 0;

		ResetParticleDisplayAttributes(particle);
		ResetParticlePositionData(particle);
		ResetParticleRotationData(particle);
	}

	void ParticleSystem::ResetParticleDisplayAttributes(Particle * particle)
	{
		if(atlasModifier != nullptr)
		{
			atlasModifier->Initialize(*particle, particle->AtlasIndex);
		}

		if(sizeModifier != nullptr)
		{
			sizeModifier->Initialize(*particle, particle->Size);
		}

		if(colorModifier != nullptr)
		{
			colorModifier->Initialize(*particle, particle->Color);
		}

		if(alphaModifier != nullptr)
		{
			alphaModifier->Initialize(*particle, particle->Alpha);
		}
	}

	void ParticleSystem::ResetParticlePositionData(Particle * particle)
	{
		particle->Position.Set(0, 0, 0);
		particle->Velocity.Set(0, 0, 0);
		particle->Acceleration.Set(0, 0, 0);

		if(positionModifier != nullptr)
		{
			positionModifier->Initialize(*particle, particle->Position);
		}

		if(!simulateInLocalSpace)
		{
			SceneObjectRef containerObject = this->GetSceneObject();
			Transform worldTransform;
			SceneObjectTransform::GetWorldTransform(worldTransform, containerObject, true, false);

			Point3 origin;
			worldTransform.TransformPoint(origin);
			Vector3 offset(origin.x, origin.y, origin.z);
			particle->Position.Add(offset);
		}

		if(velocityModifier != nullptr)
		{
			velocityModifier->Initialize(*particle, particle->Velocity);
		}

		if(accelerationModifier != nullptr)
		{
			accelerationModifier->Initialize(*particle, particle->Acceleration);
		}

	}

	void ParticleSystem::ResetParticleRotationData(Particle * particle)
	{
		particle->Rotation = 0.0f;
		particle->RotationalSpeed = 0.0f;
		particle->RotationalAcceleration = 0.0f;

		if(rotationModifier != nullptr)
		{
			rotationModifier->Initialize(*particle, particle->Rotation);
		}

		if(rotationalSpeedModifier != nullptr)
		{
			rotationalSpeedModifier->Initialize(*particle, particle->RotationalSpeed);
		}

		if(rotationalAccelerationModifier != nullptr)
		{
			rotationalAccelerationModifier->Initialize(*particle, particle->RotationalAcceleration);
		}
	}

	void ParticleSystem::AdvanceParticle(Particle* particle, Real deltaTime)
	{
		particle->Age += deltaTime;

		AdvanceParticleDisplayAttributes(particle, deltaTime);
		AdvanceParticlePositionData(particle, deltaTime);
		AdvanceParticleRotationData(particle, deltaTime);
	}

	void ParticleSystem::AdvanceParticleDisplayAttributes(Particle * particle, Real deltaTime)
	{
		if(atlasModifier != nullptr && !atlasModifier->RunOnce())
		{
			atlasModifier->Update(*particle, particle->AtlasIndex, particle->Age);
		}

		if(sizeModifier != nullptr && !sizeModifier->RunOnce())
		{
			sizeModifier->Update(*particle, particle->Size, particle->Age);
		}

		if(colorModifier != nullptr && !colorModifier->RunOnce())
		{
			colorModifier->Update(*particle, particle->Color, particle->Age);
		}

		if(alphaModifier != nullptr && !alphaModifier->RunOnce())
		{
			alphaModifier->Update(*particle, particle->Alpha, particle->Age);
		}
	}

	void ParticleSystem::AdvanceParticlePositionData(Particle * particle, Real deltaTime)
	{
		if(positionModifier != nullptr && !positionModifier->RunOnce())
		{
			positionModifier->Update(*particle, particle->Position, particle->Age);
		}
		else
		{
			Vector3 velocity = particle->Velocity;
			velocity.Scale(deltaTime);
			particle->Position.Add(velocity);
		}

		if(velocityModifier != nullptr && !velocityModifier->RunOnce())
		{
			velocityModifier->Update(*particle, particle->Velocity, particle->Age);
		}
		else
		{
			Vector3 acceleration = particle->Acceleration;
			acceleration.Scale(deltaTime);
			particle->Velocity.Add(acceleration);
		}

		if(accelerationModifier != nullptr && !accelerationModifier->RunOnce())
		{
			accelerationModifier->Update(*particle, particle->Acceleration, particle->Age);
		}
	}

	void ParticleSystem::AdvanceParticleRotationData(Particle * particle, Real deltaTime)
	{
		if(rotationModifier != nullptr && !rotationModifier->RunOnce())
		{
			rotationModifier->Update(*particle, particle->Rotation, particle->Age);
		}
		else
		{
			particle->Rotation += particle->RotationalSpeed * deltaTime;
		}

		if(rotationalSpeedModifier != nullptr && !rotationalSpeedModifier->RunOnce())
		{
			rotationalSpeedModifier->Update(*particle, particle->RotationalSpeed, particle->Age);
		}
		else
		{
			particle->RotationalSpeed += particle->RotationalAcceleration * deltaTime;
		}

		if(rotationalAccelerationModifier != nullptr && !rotationalAccelerationModifier->RunOnce())
		{
			rotationalAccelerationModifier->Update(*particle, particle->RotationalAcceleration, particle->Age);
		}
	}

	void ParticleSystem::AdvanceParticles(Real deltaTime)
	{
		UInt32 deadCount = 0;

		for(UInt32 i = 0; i < liveParticleCount; i++)
		{
			Particle* particle = liveParticleArray[i];
			AdvanceParticle(particle, deltaTime);

			if(particle->Age > particle->LifeSpan)
			{
				KillParticle(particle);
				deadCount++;
			}
		}

		if(deadCount > 0)
		{
			CleanupDeadParticles();
		}
	}

	void ParticleSystem::KillParticle(Particle * particle)
	{
		particle->Alive = false;
	}

	void ParticleSystem::ActivateParticle(Particle* particle)
	{
		ResetParticle(particle);
		particle->LifeSpan = particleLifeSpan;
		particle->Alive = true;
	}

	void ParticleSystem::ActivateParticles(UInt32 count)
	{
		for(UInt32 i = 0; i < count; i++)
		{
			if(liveParticleCount < maxParticleCount && deadParticleCount > 0)
			{
				Particle* newParticle = deadParticleArray[deadParticleCount - 1];
				liveParticleArray[liveParticleCount] = newParticle;
				deadParticleCount--;
				liveParticleCount++;

				ActivateParticle(newParticle);
			}
			else
			{
				break;
			}
		}
	}

	void ParticleSystem::CleanupDeadParticles()
	{
		Int32 topAlive = (Int32)liveParticleCount - 1;
		Int32 bottomDead = 0;
		while(topAlive > bottomDead)
		{
			while(liveParticleArray[topAlive]->Alive == false && topAlive > 0)
			{
				topAlive--;
			}

			while(liveParticleArray[bottomDead]->Alive == true && bottomDead < (Int32)liveParticleCount - 1)
			{
				bottomDead++;
			}

			if(topAlive <= bottomDead)
			{
				break;
			}

			Particle* swap = liveParticleArray[bottomDead];
			liveParticleArray[bottomDead] = liveParticleArray[topAlive];
			liveParticleArray[topAlive] = swap;
		}

		while(liveParticleCount > 0 && liveParticleArray[liveParticleCount - 1]->Alive == false)
		{
			deadParticleArray[deadParticleCount] = liveParticleArray[liveParticleCount - 1];
			deadParticleCount++;
			liveParticleCount--;
		}
	}

	void ParticleSystem::SortParticleArray(const Matrix4x4& mvpMatrix)
	{
		memcpy(_tempParticleArray, liveParticleArray, liveParticleCount * sizeof(Particle*));

		// perform full MVP projection on each particle's position so that we can sort by Z in
		// normalized device coordinates
		for(UInt32 i = 0; i < liveParticleCount; i++)
		{
			_tempParticleArray[i]->_tempPoint3 = _tempParticleArray[i]->Position;
			_tempParticleArray[i]->_tempPoint3.ApplyProjection(mvpMatrix);
		}

		QuickSortParticleArray(_tempParticleArray, 0, liveParticleCount-1);
		
		memcpy(liveParticleArray, _tempParticleArray, liveParticleCount * sizeof(Particle*));
	}

	void ParticleSystem::QuickSortParticleArray(Particle** particleArray, Int32 left, Int32 right)
	{
		if(left < right)
		{
			Int32 p = QuickSortPartition(particleArray, left, right); /* Partitioning index */
			QuickSortParticleArray(particleArray, left, p - 1);
			QuickSortParticleArray(particleArray, p + 1, right);
		}
	}

	// A utility function to swap two elements
	void ParticleSystem::QuickSortSwap(Particle** particleArray, Int32 a, Int32 b)
	{
		Particle* t = particleArray[a];
		particleArray[a] = particleArray[b];
		particleArray[b] = t;
	}

	/* This function takes last element as pivot, places the pivot element at its
	correct position in sorted array, and places all smaller (smaller than pivot)
	to left of pivot and all greater elements to right of pivot */
	UInt32 ParticleSystem::QuickSortPartition(Particle** particleArray, Int32 l, Int32 h)
	{
		Real x = particleArray[h]->_tempPoint3.z; // pivot
		Int32 i = (l - 1); // Index of smaller element
		for(Int32 j = l; j <= h - 1; j++)
		{
			// If current element is smaller than or equal to pivot 
			if(particleArray[j]->_tempPoint3.z <= x)
			{
				i++;    // increment index of smaller element
				QuickSortSwap(particleArray, i, j);  // Swap current element with index
			}
		}
		QuickSortSwap(particleArray, i + 1, h);
		return (i + 1);
	}


	Bool ParticleSystem::BindAtlasModifier(const ParticleModifier<UInt32>& modifier)
	{
		SAFE_DELETE(atlasModifier);
		atlasModifier = modifier.Clone();
		NONFATAL_ASSERT_RTRN(atlasModifier != nullptr, "ParticleSystem::BindAtlasModifier -> Unable to clone modifier.", false, false);
		return true;
	}

	Bool ParticleSystem::BindColorModifier(const ParticleModifier<Color4>& modifier)
	{
		SAFE_DELETE(colorModifier);
		colorModifier = modifier.Clone();
		NONFATAL_ASSERT_RTRN(colorModifier != nullptr, "ParticleSystem::BindColorModifier -> Unable to clone modifier.", false, false);
		return true;
	}

	Bool ParticleSystem::BindAlphaModifier(const ParticleModifier<Real>& modifier)
	{
		SAFE_DELETE(alphaModifier);
		alphaModifier = modifier.Clone();
		NONFATAL_ASSERT_RTRN(alphaModifier != nullptr, "ParticleSystem::BindAlphaModifier -> Unable to clone modifier.", false, false);
		return true;
	}

	Bool ParticleSystem::BindSizeModifier(const ParticleModifier<Vector2>& modifier)
	{
		SAFE_DELETE(sizeModifier);
		sizeModifier = modifier.Clone();
		NONFATAL_ASSERT_RTRN(sizeModifier != nullptr, "ParticleSystem::BindSizeModifier -> Unable to clone modifier.", false, false);
		return true;
	}

	Bool ParticleSystem::BindPositionModifier(const ParticleModifier<Point3>& modifier)
	{
		SAFE_DELETE(positionModifier);
		positionModifier = modifier.Clone();
		NONFATAL_ASSERT_RTRN(positionModifier != nullptr, "ParticleSystem::BindPositionModifier -> Unable to clone modifier.", false, false);
		return true;
	}

	Bool ParticleSystem::BindVelocityModifier(const ParticleModifier<Vector3>& modifier)
	{
		SAFE_DELETE(velocityModifier);
		velocityModifier = modifier.Clone();
		NONFATAL_ASSERT_RTRN(velocityModifier != nullptr, "ParticleSystem::BindVelocityModifier -> Unable to clone modifier.", false, false);
		return true;
	}

	Bool ParticleSystem::BindAccelerationModifier(const ParticleModifier<Vector3>& modifier)
	{
		SAFE_DELETE(accelerationModifier);
		accelerationModifier = modifier.Clone();
		NONFATAL_ASSERT_RTRN(accelerationModifier != nullptr, "ParticleSystem::BindAccelerationModifier -> Unable to clone modifier.", false, false);
		return true;
	}

	Bool ParticleSystem::BindRotationModifier(const ParticleModifier<Real>& modifier)
	{
		SAFE_DELETE(rotationModifier);
		rotationModifier = modifier.Clone();
		NONFATAL_ASSERT_RTRN(rotationModifier != nullptr, "ParticleSystem::BindRotationModifier -> Unable to clone modifier.", false, false);
		return true;
	}

	Bool ParticleSystem::BindRotationalSpeedModifier(const ParticleModifier<Real>& modifier)
	{
		SAFE_DELETE(rotationalSpeedModifier);
		rotationalSpeedModifier = modifier.Clone();
		NONFATAL_ASSERT_RTRN(rotationalSpeedModifier != nullptr, "ParticleSystem::BindRotationalSpeedModifier -> Unable to clone modifier.", false, false);
		return true;
	}

	Bool ParticleSystem::BindRotationalAccelerationModifier(const ParticleModifier<Real>& modifier)
	{
		SAFE_DELETE(rotationalAccelerationModifier);
		rotationalAccelerationModifier = modifier.Clone();
		NONFATAL_ASSERT_RTRN(rotationalAccelerationModifier != nullptr, "ParticleSystem::BindRotationalAccelerationModifier -> Unable to clone modifier.", false, false);
		return true;
	}

	void ParticleSystem::SetPremultiplyAlpha(Bool premultiply)
	{
		premultiplyAlpha = premultiply;
	}

	void ParticleSystem::SetZSort(Bool sort)
	{
		zSort = sort;
	}

	SceneObjectRef ParticleSystem::GetMeshSceneObject()
	{
		return meshObject;
	}
}
