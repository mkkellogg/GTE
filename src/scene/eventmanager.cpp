#include "eventmanager.h"
#include "sceneobject.h"
#include "sceneobjectcomponent.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/particles/particlesystem.h"
#include "global/assert.h"

namespace GTE {
    EventManager::EventManager() {

    }

    EventManager::~EventManager() {

    }

    Bool EventManager::Init() {
        return true;
    }

    void EventManager::DispatchSceneObjectComponentEvent(SceneObjectEvent evnt, SceneObjectComponent& component) {
        SceneObjectRef container = component.GetSceneObject();
        NONFATAL_ASSERT(container.IsValid(), "EventManager::DispatchSceneObjectComponentEvent -> Component's container is not valid!", true);

        // only process components that belong to active scene objects
        if (container->IsActive()) {
            SceneObjectComponentProcessingDescriptor& processingDesc = component.GetProcessingDescriptor();
            switch (evnt) {
                case SceneObjectEvent::Awake:
                if (!processingDesc.Awake) {
                    component.Awake();
                    processingDesc.Awake = true;
                }
                break;
                case SceneObjectEvent::Start:
                if (!processingDesc.Started) {
                    component.Start();
                    processingDesc.Started = true;
                }
                break;
                case SceneObjectEvent::Update:
                component.Update();
                break;
                case SceneObjectEvent::PreRender:
                component.PreRender();
                break;
                case SceneObjectEvent::WillRender:
                component.WillRender();
                break;
            }
        }
    }

    void EventManager::DispatchSceneObjectEvent(SceneObjectEvent evnt, SceneObject& object) {
        // only process active scene objects
        if (object.IsActive()) {
            CameraRef camera = object.GetCamera();
            if (camera.IsValid()) {
                DispatchSceneObjectComponentEvent(evnt, camera.GetRef());
            }

            LightRef light = object.GetLight();
            if (light.IsValid()) {
                DispatchSceneObjectComponentEvent(evnt, light.GetRef());
            }

            Mesh3DFilterRef filter = object.GetMesh3DFilter();
            if (filter.IsValid()) {
                DispatchSceneObjectComponentEvent(evnt, filter.GetRef());
            }

            RendererRef renderer = object.GetRenderer();
            if (renderer.IsValid()) {
                DispatchSceneObjectComponentEvent(evnt, renderer.GetRef());
            }

            ParticleSystemRef particleSystem = object.GetParticleSystem();
            if (particleSystem.IsValid()) {
                DispatchSceneObjectComponentEvent(evnt, particleSystem.GetRef());
            }
        }
    }
}
