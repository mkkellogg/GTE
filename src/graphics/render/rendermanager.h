/*
 * class:RenderManager
 *
 * author: Mark Kellogg
 *
 * Base class for all render managers.
 */

#ifndef _GTE_RENDER_MANAGER_H
#define _GTE_RENDER_MANAGER_H

#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <stack>
#include "object/engineobject.h"
#include "object/enginetypes.h"

namespace GTE
{
	enum class RenderQueueType
	{
		Opaque = 1000,
		OpaquePostSSAO = 2000,
		Skybox = 3000,
		Transparent = 4000,
		MaxQueue = 10000
	};

	class RenderManager
	{
		friend class Engine;

		protected:

		RenderManager();
		virtual ~RenderManager();

		virtual void PreRender() = 0;

		CameraSharedPtr currentCamera;
		void SetCurrentCamera(CameraRef camera);

		public:
		
		CameraRef GetCurrentCamera();

		virtual void RenderScene() = 0;
		virtual void ClearCaches() = 0;

		virtual void RenderFullScreenQuad(RenderTargetRef renderTarget, MaterialRef material, Bool clearBuffers) = 0;
	};
}

#endif
