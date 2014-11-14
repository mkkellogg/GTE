#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <memory>
#include "asset/assetimporter.h"
#include "graphics/graphics.h"
#include "graphics/stdattributes.h"
#include "graphics/object/submesh3D.h"
#include "graphics/animation/skeleton.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/animationmanager.h"
#include "graphics/animation/animationinstance.h"
#include "graphics/animation/animationplayer.h"
#include "graphics/animation/bone.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/renderbuffer.h"
#include "graphics/render/material.h"
#include "graphics/shader/shader.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "graphics/texture/textureattr.h"
#include "graphics/texture/texture.h"
#include "graphics/color/color4.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2.h"
#include "graphics/uv/uv2array.h"
#include "geometry/matrix4x4.h"
#include "base/basevector4.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3array.h"
#include "ui/debug.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "util/time.h"
#include "gte.h"
#include "game/game.h"

class CustomGraphicsCallbacks: public GraphicsCallbacks
{
	public:

	Game * game;

	CustomGraphicsCallbacks()
	{
		game = NULL;
	}

	void PrintMatrix(Matrix4x4 *m)
	{
		const float * data = m->GetDataPtr();
		for (int r = 0; r < 4; r++)
		{
			printf("[");
			for (int c = 0; c < 4; c++)
			{
				if (c > 0)
					printf(",");
				printf("%f", data[c * 4 + r]);
			}
			printf("]\n");
		}
	}

	void PrintVector(BaseVector4 * vector)
	{
		float * data = vector->GetDataPtr();
		printf("[");
		for (int c = 0; c < 4; c++)
		{
			if (c > 0)
				printf(",");
			printf("%f", data[c]);
		}
		printf("]\n");
	}

	void OnInit(Graphics * graphics)
	{
		game = new Game();
		game->Init();
	}

	void OnUpdate(Graphics * graphics)
	{
		game->Update();
	}

	SkinnedMesh3DRendererRef FindFirstSkinnedMeshRenderer(SceneObjectRef ref)
	{
		if(!ref.IsValid())return SkinnedMesh3DRendererRef::Null();

		if(ref->GetSkinnedMesh3DRenderer().IsValid())return ref->GetSkinnedMesh3DRenderer();

		for(unsigned int i = 0; i < ref->GetChildrenCount(); i++)
		{
			SceneObjectRef childRef = ref->GetChildAt(i);
			SkinnedMesh3DRendererRef subRef = FindFirstSkinnedMeshRenderer(childRef);
			if(subRef.IsValid())return subRef;
		}

		return SkinnedMesh3DRendererRef::Null();
	}

	void OnQuit(Graphics * graphics)
	{

	}

	virtual ~CustomGraphicsCallbacks()
	{

	}
};

int main(int argc, char** argv)
{
	CustomGraphicsCallbacks graphicsCallbacks;
	bool initSuccess = Graphics::Instance()->Init(1280, 800,(CustomGraphicsCallbacks*) (&graphicsCallbacks), "GTE Test");
	if(initSuccess)
	{
		Graphics::Instance()->Run();
	}
	else
	{
		Debug::PrintError("Error occurred while initializing engine.");
	}

	return EXIT_SUCCESS;
}

