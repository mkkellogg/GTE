/*
 * class:RenderManager
 *
 * author: Mark Kellogg
 *
 * Base clss for all render managers.
 */

#ifndef _GTE_RENDER_MANAGER_H
#define _GTE_RENDER_MANAGER_H

#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <stack>
#include "rendermanager.h"
#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "object/objectpairkey.h"
#include "util/datastack.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "geometry/transform.h"

namespace GTE
{
	// forward declaration
	class SceneObject;
	class Graphics;
	class EngineObjectManager;
	class Transform;
	class Material;
	class SceneObjectComponent;
	class SubMesh3D;
	class Transform;
	class Point3Array;


	class RenderManager
	{
		friend class Engine;

		virtual void Update() = 0;

	public:

		RenderManager();
		virtual ~RenderManager();

		virtual void RenderScene() = 0;
		virtual void ClearCaches() = 0;

		virtual void RenderFullScreenQuad(RenderTargetRef renderTarget, MaterialRef material, Bool clearBuffers) = 0;
	};
}

#endif
