/*
 * class: Engine
 *
 * author: Mark Kellogg
 *
 * This is the central top-level class of the engine. This class instantiates many core
 * components of the engine and stores references to them and manages their life-cycle.
 *
 * The singleton instance [theInstance] is accessible at any time publicly. However,
 * before Engine can be used, it must be initialized through a call to Init(). Additionally,
 * the Start() method must be called to start the update loop.
 */

#ifndef _GTE_ENGINE_H_
#define _GTE_ENGINE_H_

#include <string>

#include "enginetypes.h"

namespace GTE {
    //forward declarations
    class Graphics;
    class EngineObjectManager;
    class AnimationManager;
    class SceneManager;
    class InputManager;
    class ErrorManager;
    class GraphicsAttributes;
    class RenderManager;
    class EventManager;

    class EngineCallbacks {
    public:

        virtual void OnAwake() = 0;
        virtual void OnStart() = 0;
        virtual void OnQuit() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnPreRender() = 0;
        virtual ~EngineCallbacks();
    };

    class Engine {
        friend class Graphics;
        friend class GraphicsGL;

        // Singleton instance of Engine
        static Engine * theInstance;

        // Manages all objects that derive from EngineObject
        EngineObjectManager * engineObjectManager;

        // Interface for all lower-level graphics-related functionality
        Graphics * graphicsSystem;

        // Manages the rendering of the scene
        RenderManager * renderManager;

        // Manages active animations
        AnimationManager * animationManager;

        // manage scene objects
        SceneManager * sceneManager;

        // Manages user input
        InputManager * inputManager;

        // Manages/reports errors reported by any engine component
        ErrorManager * errorManager;

        // Manages dispatching of engine and object events
        EventManager * eventManager;

        // Registered call-backs for engine life-cycle events
        EngineCallbacks * callbacks;

        // flag that indicates the engine has been initialized
        Bool initialized;

        Bool firstFrameEntered;

        Engine();
        ~Engine();

        void Update();
        void Quit();
        Bool _Init(EngineCallbacks * callbacks, const GraphicsAttributes& graphicsAtrributes);
        void _Start();

    public:

        static Engine * Instance();
        static void ShutDown();
        static Bool Init(EngineCallbacks * callbacks, const GraphicsAttributes& graphicsAtrributes);
        static void Start();

        Bool IsInitialized() const;

        EngineObjectManager * GetEngineObjectManager();
        Graphics * GetGraphicsSystem();
        RenderManager * GetRenderManager();
        AnimationManager * GetAnimationManager();
        InputManager * GetInputManager();
        ErrorManager * GetErrorManager();
        EventManager * GetEventManager();
        SceneManager * GetSceneManager();
    };
}

#endif
