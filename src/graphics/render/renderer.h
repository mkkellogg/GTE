/*
 * class: Renderer
 *
 * author: Mark Kellogg
 *
 * Base class for all renderers
 */

#ifndef _GTE_RENDERER_H_
#define _GTE_RENDERER_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "object/sceneobjectcomponent.h"
#include <vector>

namespace GTE
{
	//forward declarations
	class EngineObjectManager;

	class Renderer : public SceneObjectComponent
	{
		// Since this ultimately derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;

	protected:

		Renderer();
		virtual ~Renderer();

	public:

	};
}

#endif
