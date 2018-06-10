#include <memory.h>

#include "scenemanager.h"
#include "object/engineobject.h"
#include "scene/sceneobject.h"
#include "scene/sceneobjectcomponent.h"
#include "object/engineobjectmanager.h"
#include "scene/eventmanager.h"
#include "geometry/sceneobjecttransform.h"
#include "geometry/vector/vector3.h"
#include "geometry/quaternion.h"
#include "geometry/transform.h"
#include "geometry/matrix4x4.h"
#include "global/global.h"
#include "global/assert.h"
#include "global/constants.h"
#include "util/datastack.h"
#include "debug/gtedebug.h"

namespace GTE {
    /*
    * Default constructor
    */
    SceneManager::SceneManager() {
        sceneObjectCount = 0;
        maxPhaseReached = -1;
    }

    /*
     * Destructor
     */
    SceneManager::~SceneManager() {

    }



    /*
    * Initialize. Return false if initialization false, true if it succeeds.
    */
    Bool SceneManager::Init() {
        return true;
    }

    /*
    * Called once when the scene manager first starts.
    */
    void SceneManager::Awake() {
        Update(UpdatePhase::Awake);
        maxPhaseReached = (Int32)UpdatePhase::Awake;
    }

    /*
    * Called once when the scene manager first starts, but after Awake().
    */
    void SceneManager::Start() {
        Update(UpdatePhase::Start);
        maxPhaseReached = (Int32)UpdatePhase::Start;
    }

    /*
     * Kick off scene processing from the scene root.
     */
    void SceneManager::Update() {
        Update(UpdatePhase::Update);
        maxPhaseReached = (Int32)UpdatePhase::Update;
    }

    /*
    * Kick off scene processing from the scene root.
    */
    void SceneManager::Update(UpdatePhase phase) {
        SceneObjectRef sceneRoot = Engine::Instance()->GetEngineObjectManager()->GetSceneRoot();
        ASSERT(sceneRoot.IsValid(), "SceneManager::Update -> 'sceneRoot' is null.");

        sceneObjectCount = 0;

        Transform baseTransform;
        // form list of scene objects
        ProcessScene(sceneRoot.GetRef(), baseTransform);
        // process resulting list of scene objects
        ProcessSceneObjectList(phase);
    }

    /*
    *
    * Recursively visits each object in the scene that is reachable from [parent]. The transforms of each
    * scene object are concatenated as progress moves down the scene object tree and passed to the current
    * invocation via [aggregateTransform]. These aggregate transforms are saved to each SceneObject via
    * SceneObject::SetAggregateTransform().
    *
    */
    void SceneManager::ProcessScene(SceneObject& obj, Transform& aggregateTransform) {
        Transform nextTransform = aggregateTransform;
        Transform& localTransform = obj.GetTransform();
        if (!obj.InheritsTransform()) {
            nextTransform.SetIdentity();
        }
        nextTransform.TransformBy(localTransform);

        // save the aggregate/global/world transform
        SceneObjectProcessingDescriptor& processingDesc = obj.GetProcessingDescriptor();
        processingDesc.AggregateTransform = nextTransform;
        processingDesc.AggregateTransformInverse = nextTransform;
        processingDesc.AggregateTransformInverse.Invert();

        sceneObjectList[sceneObjectCount] = &obj;
        sceneObjectCount++;

        for (UInt32 i = 0; i < obj.GetChildrenCount(); i++) {
            SceneObjectRef child = obj.GetChildAt(i);

            if (!child.IsValid()) {
                Debug::PrintWarning("SceneManager::PreProcessScene -> Null scene object encountered.");
                continue;
            }

            // only process active scene objects
            if (child->IsActive()) {
                if (sceneObjectCount >= Constants::MaxSceneObjects)return;
                // continue recursion through child object
                ProcessScene(child.GetRef(), nextTransform);
            }
        }
    }

    void SceneManager::ProcessSceneObjectList(UpdatePhase phase) {
        for (UInt32 i = 0; i < sceneObjectCount; i++) {
            SceneObject * sceneObject = sceneObjectList[i];

            ProcessSceneObjectUpdatePhase(phase, *sceneObject);
        }
    }

    void SceneManager::ProcessSceneObjectUpdatePhase(UpdatePhase phase, SceneObject& object) {
        switch (phase) {
            case UpdatePhase::Awake:
            Engine::Instance()->GetEventManager()->DispatchSceneObjectEvent(SceneObjectEvent::Awake, object);
            break;
            case UpdatePhase::Start:
            Engine::Instance()->GetEventManager()->DispatchSceneObjectEvent(SceneObjectEvent::Start, object);
            break;
            case UpdatePhase::Update:
            Engine::Instance()->GetEventManager()->DispatchSceneObjectEvent(SceneObjectEvent::Update, object);
            break;
        }
    }

    void SceneManager::ProcessSceneObjectComponentUpdatePhase(UpdatePhase phase, SceneObjectComponent& component) {
        switch (phase) {
            case UpdatePhase::Awake:
            Engine::Instance()->GetEventManager()->DispatchSceneObjectComponentEvent(SceneObjectEvent::Awake, component);
            break;
            case UpdatePhase::Start:
            Engine::Instance()->GetEventManager()->DispatchSceneObjectComponentEvent(SceneObjectEvent::Start, component);
            break;
            case UpdatePhase::Update:
            Engine::Instance()->GetEventManager()->DispatchSceneObjectComponentEvent(SceneObjectEvent::Update, component);
            break;
        }
    }

    /*
    * Fully process a SceneObject instance as if it were newly added.
    */
    void SceneManager::ProcessSceneObjectAsNew(SceneObject& object) {
        // only process active scene objects
        if (object.IsActive()) {
            Transform aggregateTransform;
            SceneObjectTransform::GetWorldTransform(aggregateTransform, object, true, false);

            // save the aggregate/global/world transform
            SceneObjectProcessingDescriptor& processingDesc = object.GetProcessingDescriptor();
            processingDesc.AggregateTransform = aggregateTransform;
            processingDesc.AggregateTransformInverse = aggregateTransform;
            processingDesc.AggregateTransformInverse.Invert();

            if (maxPhaseReached >= (Int32)UpdatePhase::Awake) {
                ProcessSceneObjectUpdatePhase(UpdatePhase::Awake, object);
            }

            if (maxPhaseReached >= (Int32)UpdatePhase::Start) {
                ProcessSceneObjectUpdatePhase(UpdatePhase::Start, object);
            }
        }
    }

    /*
    * Fully process a SceneObjectComponent instance as if it were newly added.
    */
    void SceneManager::ProcessSceneObjectComponentAsNew(SceneObjectComponent& component) {
        SceneObjectRef container = component.GetSceneObject();
        NONFATAL_ASSERT(container.IsValid(), "SceneManager::ProcessSceneObjectComponentAsNew -> Component's container is not valid!", true);

        // only process components that belong to active scene objects
        if (container->IsActive()) {
            if (maxPhaseReached >= (Int32)UpdatePhase::Awake) {
                ProcessSceneObjectComponentUpdatePhase(UpdatePhase::Awake, component);
            }

            if (maxPhaseReached >= (Int32)UpdatePhase::Start) {
                ProcessSceneObjectComponentUpdatePhase(UpdatePhase::Start, component);
            }
        }
    }

}
