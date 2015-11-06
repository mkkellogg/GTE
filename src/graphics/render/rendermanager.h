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
#include "object/engineobject.h"
#include "object/enginetypes.h"

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

	enum class RenderQueueType
	{
		Geometry = 1000,
		Transparent = 2000
	};

	class RenderManager
	{
		friend class Engine;

		virtual void Update() = 0;

		public:

		RenderManager();
		virtual ~RenderManager();

		virtual void RenderScene() = 0;
		virtual void ClearCaches() = 0;

		virtual void RenderFullScreenQuad(const RenderTargetSharedPtr& renderTarget, const MaterialSharedPtr& material, Bool clearBuffers) = 0;
	};
}

#endif
