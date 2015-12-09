/*
* class: ViewDescriptor
*
* author: Mark Kellogg
*
* Data structure that is used by render managers to describe camera/view properties
* such as the view transform, inverse view transform, view position, and several others.
*/

#ifndef _GTE_VIEW_DESCRIPTOR_H_
#define _GTE_VIEW_DESCRIPTOR_H_

#include "engine.h"
#include "graphics/graphicsattr.h"
#include "object/engineobject.h"
#include "geometry/transform.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "base/bitmask.h"

namespace GTE
{
	class ViewDescriptor
	{
		public:

		IntMask ClearBufferMask;
		IntMask CullingMask;

		Bool ReverseCulling;

		Transform ViewTransform;
		Transform ViewTransformInverse;

		Point3 ViewPosition;

		Transform ProjectionTransform;
		Transform ProjectionTransformInverse;
		Transform UniformWorldSceneObjectTransform;
		Transform UniformWorldSceneObjectTransformInverse;

		Bool LightingEnabled;
		Bool DepthPassEnabled;

		Bool SSAOEnabled;
		SSAORenderMode SSAOMode;

		Bool SkyboxEnabled;
		SceneObject * SkyboxObject;

		UInt32 ClipPlaneCount;
		Vector3 ClipPlane0Normal;
		Real ClipPlane0Offset;

		ViewDescriptor();
		~ViewDescriptor();
	};
}

#endif
