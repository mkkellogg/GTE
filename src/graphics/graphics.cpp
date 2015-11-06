#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "engine.h"
#include "graphics.h"
#include "stdattributes.h"
#include "stduniforms.h"
#include "global/global.h"
#include "global/assert.h"
#include "texture/atlas.h"
#include "shader/shader.h"
#include "render/material.h"
#include "geometry/transform.h"
#include "render/submesh3Drenderer.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "global/global.h"
#include "debug/gtedebug.h"
#include "util/time.h"

namespace GTE
{
	/*
	* Base constructor, initialize member variables.
	*/
	Graphics::Graphics()
	{
		currentFPSSpanTime = 0;
		framesInFPSSpan = 0;
		currentFPS = 0.0;
	}

	/*
	 * Clean up.
	 */
	Graphics::~Graphics()
	{

	}

	/*
	 * Create default render target and wrap in a RenderTargetRef reference.
	 */
	RenderTargetSharedPtr Graphics::SetupDefaultRenderTarget()
	{
		// get reference to the engine's object manager
		EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

		RenderTarget * defaultTarget = CreateDefaultRenderTarget();
		ASSERT(defaultTarget != nullptr, "GraphicsGL::SetupDefaultRenderTarget -> Default target is null.");

		RenderTargetSharedPtr defaultRenderTarget = objectManager->WrapRenderTarget(defaultTarget);
		ASSERT(defaultRenderTarget.IsValid(), "GraphicsGL::SetupDefaultRenderTarget -> Default target is null.");

		return defaultRenderTarget;
	}

	/*
	 * For now this method does nothing. It is meant to be overridden in a
	 * deriving class. It is not purely virtual because it will likely contain code
	 * as the Graphics class evolves.
	 */
	Bool Graphics::Init(const GraphicsAttributes& attributes)
	{
		StandardAttributes::RegisterAll();
		StandardUniforms::RegisterAll();

		return true;
	}

	/*
	 * Update is called once per frame.
	 */
	void Graphics::Update()
	{
		UpdateFPS();
	}

	/*
	 * Update the FPS calculation.
	 */
	void Graphics::UpdateFPS()
	{
		currentFPSSpanTime += Time::GetDeltaTime();
		framesInFPSSpan++;
		if (currentFPSSpanTime >= 1)
		{
			currentFPS = (Real)framesInFPSSpan / currentFPSSpanTime;
			//printf("fps: %f\n", currentFPS);
			currentFPSSpanTime = 0;
			framesInFPSSpan = 0;
		}
	}

	/*
	 * Called when the graphics interface starts up. Convenience method for
	 * deriving classes to receive a signal when the engine starts.
	 */
	Bool Graphics::Start()
	{
		return true;
	}

	/*
	 * Called when the graphics interface shuts down. Convenience method for
	 * deriving classes to receive a signal when the engine shuts down.
	 */
	void Graphics::End()
	{

	}

	/*
	 * In this base class, this method does nothing. Deriving classes can override this
	 * method and perform any special functionality that may be necessary after rendering
	 * the scene, such as swapping buffers in a double buffering situation.
	 */
	void Graphics::PostRender()
	{

	}

	/*
	 * Get the currently calculated FPS value.
	 */
	Real Graphics::GetCurrentFPS() const
	{
		return currentFPS;
	}

	/*
	 * Get the currently active graphics properties.
	 */
	const GraphicsAttributes& Graphics::GetAttributes() const
	{
		return attributes;
	}

	/*
	* Create an Atlas object that partitions [texture] in multiple sub-frames.
	*
	* [createFirstFullFrame] - If true, create initial frame that covers all of [texture].
	*/
	Atlas * Graphics::CreateAtlas(TextureSharedPtr texture, Bool createFirstFullFrame)
	{
		Atlas * atlas = new(std::nothrow)Atlas(texture, createFirstFullFrame);
		ASSERT(atlas != nullptr, "Graphics::CreateAtlas -> Unable to allocate new atlas.");
		return atlas;
	}

	/*
	* Destroy the Atlas object specified by [atlas].
	*/
	void Graphics::DestroyAtlas(Atlas * atlas)
	{
		NONFATAL_ASSERT(atlas != nullptr, "Graphics::DestroyAtlas -> 'atlas' is null", true);
		delete atlas;
	}
}

