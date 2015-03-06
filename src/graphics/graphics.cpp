#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "engine.h"
#include "graphics.h"
#include "global/global.h"
#include "graphicsGL.h"
#include "shader/shader.h"
#include "render/material.h"
#include "geometry/transform.h"
#include "render/submesh3Drenderer.h"
#include "render/rendermanager.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "global/global.h"
#include "debug/gtedebug.h"
#include "util/time.h"

/*
 * Base constructor, initialize member variables.
 */
Graphics::Graphics()
{
    currentFPSSpanTime =0;
	framesInFPSSpan =0;
	currentFPS = 0.0;
}

/*
 * Clean up.
 */
Graphics::~Graphics()
{

}

/*
 * For now this method does nothing. It is meant to be overridden in a
 * deriving class. It is not virtual because it will likely contain code
 * as the Graphics class evolves.
 */
bool Graphics::Init(const GraphicsAttributes& attributes)
{
	return true;
}

/*
 * For now this method does nothing. It is not virtual because it will likely contain code
 * as the Graphics class evolves.
 */
void Graphics::PreProcessScene()
{

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
	if(currentFPSSpanTime >= 1)
	{
		currentFPS = (float)framesInFPSSpan/currentFPSSpanTime;
		//printf("fps: %f\n", currentFPS);
		currentFPSSpanTime = 0;
		framesInFPSSpan = 0;
	}
}

/*
 * Called when the graphics engine starts up. Convenience method for
 * deriving classes to receive a signal when the engine starts.
 */
bool Graphics::Start()
{
	return true;
}

/*
 * In this base class, this is merely a pass-thru/middleman method that
 * calls RenderAll() in the render manager. Deriving classes can override this
 * method and perform any special functionality that may be necessary when rendering
 * the scene, such as swapping buffers in a double buffering situation.
 */
void Graphics::RenderScene()
{
	Engine::Instance()->GetRenderManager()->RenderScene();
}

/*
 * Set the material (and shader) that should be used for rendering.
 */
void Graphics::ActivateMaterial(MaterialRef material)
{
	activeMaterial = material;
	material->ResetVerificationState();
}

/*
 * Get the material that is currently being used for rendering.
 */
MaterialRef Graphics::GetActiveMaterial() const
{
	return activeMaterial;
}

/*
 * Get the currently calculated FPS value.
 */
float Graphics::GetCurrentFPS()
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
 * Turn on/off SSAO.
 */
void Graphics::SetSSAOEnabled(bool enabled)
{
	attributes.SSAOEnabled = enabled;
}

/*
 * Set the type of SSAO being used.
 */
void Graphics::SetSSAOMode(SSAORenderMode mode)
{
	attributes.SSAOMode = mode;
}

