#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "particlesystem.h"
#include "particleutil.h"
#include "particle.h"
#include "object/enginetypes.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "framesetmodifier.h"
#include "randommodifier.h"
#include "evenintervalindexmodifier.h"
#include "graphics/shader/shadersource.h"
#include "graphics/render/material.h"
#include "graphics/renderstate.h"
#include "graphics/stdattributes.h"
#include "graphics/object/submesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "asset/assetimporter.h"
#include "engine.h"

namespace GTE
{
	ParticleSystem::ParticleSystem()
	{
		zSort = false;
		simulateInLocalSpace = true;
	
		releaseAtOnce = false;
		releaseAtOnceCount = 0;
		hasInitialReleaseOccurred = false;
		isActive = false;

		atlasModifier = nullptr;
		colorModifier = nullptr;
		alphModifier = nullptr;
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
		lifespan = 0.0f;
	}

	ParticleSystem::~ParticleSystem()
	{
		Destroy();
	}

	void ParticleSystem::Destroy()
	{

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
			if(lifespan != 0 && lifespan  < minLifeSpan) minLifeSpan = lifespan;
			maxParticleCount = (UInt32)GTEMath::Max(particleReleaseRate * minLifeSpan * 2, 1.0f);
		}

		vertexCount = maxParticleCount * (UInt32)ParticleConstants::VerticesPerParticle;
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
		
		SubMesh3DSharedPtr subMesh = objectManager->CreateSubMesh3D(meshAttributes);
		subMesh->Init(vertexCount);

		mesh = objectManager->CreateMesh3D(1);
		mesh->Init();
		mesh->SetSubMesh(subMesh, 0);
		mesh->Update();

		Bool bufferInitSuccess = subMesh->AddCustomFloatAttributeBuffer(2, "size");
		NONFATAL_ASSERT_RTRN(bufferInitSuccess == true, "ParticleSystem::InitializeMesh -> Could not add custom attribute 'size'.", false, false);

		bufferInitSuccess = subMesh->AddCustomFloatAttributeBuffer(1, "rotation");
		NONFATAL_ASSERT_RTRN(bufferInitSuccess == true, "ParticleSystem::InitializeMesh -> Could not add custom attribute 'rotation'.", false, false);

		bufferInitSuccess = subMesh->AddCustomFloatAttributeBuffer(1, "customIndex");
		NONFATAL_ASSERT_RTRN(bufferInitSuccess == true, "ParticleSystem::InitializeMesh -> Could not add custom attribute 'customIndex'.", false, false);

		Mesh3DFilterSharedPtr filter = objectManager->CreateMesh3DFilter();
		NONFATAL_ASSERT_RTRN(filter.IsValid(), "ParticleSystem::InitializeMesh -> Could not create mesh filter.", false, false);
		meshObject->SetMesh3DFilter(filter);
		filter->SetMesh3D(mesh);
		filter->SetCastShadows(false);
		filter->SetReceiveShadows(false);

		Mesh3DRendererSharedPtr meshRenderer = objectManager->CreateMesh3DRenderer();
		NONFATAL_ASSERT_RTRN(meshRenderer.IsValid(), "ParticleSystem::InitializeMesh -> Could not create mesh renderer.", false, false);
		meshRenderer->AddMaterial(particleMaterial);
		meshObject->SetMesh3DRenderer(meshRenderer);

		return true;
	}

	void ParticleSystem::DestroyMesh()
	{
		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

		NONFATAL_ASSERT(meshObject.IsValid(), "ParticleSystem::DestroyMesh -> Mesh object is not valid.", true);

		Mesh3DFilterRef meshFilter = meshObject->GetMesh3DFilter();
		NONFATAL_ASSERT(meshObject.IsValid(), "ParticleSystem::DestroyMesh -> Mesh filter object is not valid.", true);

		meshFilter->RemoveMesh3D();
		meshObject->RemoveMesh3DFilter();
		meshObject->RemoveMesh3DRenderer();

		if(!mesh.IsValid())return;
		
		objectManager->DestroyMesh3D(mesh);

		mesh = Mesh3DSharedPtr::Null();
	}
}
