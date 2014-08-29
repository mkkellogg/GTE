
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "graphics/graphics.h"
#include "graphics/attributes.h"
#include "graphics/object/mesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/renderbuffer.h"
#include "graphics/shader/shader.h"
#include "graphics/view/camera.h"
#include "geometry/matrix4x4.h"
#include "base/basevector4.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3array.h"
#include "graphics/color/color4.h"
#include "graphics/color/color4array.h"
#include "ui/debug.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "gte.h"


class CustomGraphicsCallbacks : public GraphicsCallbacks
{
    public : 

	SceneObject * cameraObject;
    CustomGraphicsCallbacks()
    {
    	cameraObject = NULL;
    }

    void PrintMatrix(Matrix4x4 *m)
    {
        const float * data = m->GetDataPtr();
        for(int r=0; r < 4; r++)
        {
            printf("[");
            for(int c=0; c < 4; c++)
            {   
                if(c>0)printf(",");
                printf("%f", data[c*4+r]);
            }
            printf("]\n");
        }
    }

    void PrintVector(BaseVector4 * vector)
    {
        float * data = vector->GetDataPtr();
        printf("[");
        for(int c=0; c < 4; c++)
        {   
            if(c>0)printf(",");
            printf("%f", data[c]);
        }
        printf("]\n");
    }


    void OnInit(Graphics * graphics)
    {

        /*Point3 *point = new Point3(0,1,2);

        const float * data = point->GetData();

        for(int i=0; i < 4; i++)printf("[%f]", data[i]);
        printf("\n");

        point->x = 5;
        point->y = 6;
        point->z = 7;
        point->w = 8;

        for(int i=0; i < 4; i++)printf("[%f]", data[i]);
        printf("\n");

        delete point;*/

       /* float dataA[] = {1,0,0,0,
                         0,1,0,0,
                         0,0,1,0,
                         4,4,4,1};

        float dataB[] = {1,0,0,0,
                         0,1,0,0,
                         0,0,1,0,
                         1,2,3,1};

        Matrix4x4 a(dataA);
        Matrix4x4 b(dataB);
        Matrix4x4 c;  

        Matrix4x4::Multiply(&a, &b, &c);        

        PrintMatrix(&c);   

        printf("\n");

        Point3 va(1,1,1);
        Point3 vb(0,0,0);

        c.Transform(&va, &vb);

        PrintVector(&vb);

        vb.AttachTo(va.GetDataPtr());

        PrintVector(&vb);

        Point3Array pointArray;

        pointArray.Init(32);

        for(int i=0; i< 32; i++)
        {
        	Point3 * p = pointArray.GetPoint(i);
        	p->Set(2,2,2);
        	a.Transform(p);
        	PrintVector(p);
        }*/

        EngineObjectManager * objectManager = EngineObjectManager::Instance();

        AttributeSet meshAttributes = Attributes::CreateAttributeSet();
        Attributes::AddAttribute(&meshAttributes, Attribute::Position);
        Attributes::AddAttribute(&meshAttributes, Attribute::Color);

        cameraObject = objectManager->CreateSceneObject();
        Camera * camera = objectManager->CreateCamera();
        cameraObject->GetTransform()->Translate(0,5,5,true);
       // cameraObject->GetTransform()->RotateAround(0,0,-12,0,1,0,90);
        camera->AddClearBuffer(RenderBufferType::Color);
        camera->AddClearBuffer(RenderBufferType::Depth);
        cameraObject->SetCamera(camera);

        SceneObject * sceneObject = objectManager->CreateSceneObject();
        sceneObject->GetTransform()->RotateAround(0,0,0,0,1,0,45);
        sceneObject->GetTransform()->Translate(0,0,-12,false);
        Material * material = objectManager->CreateMaterial("resources/basic.vertex.shader","resources/basic.fragment.shader");
        Mesh3D * mesh = objectManager->CreateMesh3D(meshAttributes);
        Mesh3DRenderer * meshRenderer = objectManager->CreateMesh3DRenderer();

        meshRenderer->UseMaterial(material);

        sceneObject->SetMesh(mesh);
        sceneObject->SetMeshRenderer(meshRenderer);

        mesh->Init(36);

        Point3Array * points = mesh->GetPostions();
        Color4Array * colors = mesh->GetColors();

        // --- Cube vertices -------
        // cube front, triangle 1
        points->GetPoint(0)->Set(-1,1,1);
        points->GetPoint(1)->Set(1,1,1);
        points->GetPoint(2)->Set(-1,-1,1);

        // cube front, triangle 2
        points->GetPoint(3)->Set(1,1,1);
        points->GetPoint(4)->Set(1,-1,1);
        points->GetPoint(5)->Set(-1,-1,1);

        // cube right, triangle 1
	   points->GetPoint(6)->Set(1,1,1);
	   points->GetPoint(7)->Set(1,1,-1);
	   points->GetPoint(8)->Set(1,-1,1);

	   // cube right, triangle 2
	   points->GetPoint(9)->Set(1,-1,1);
	   points->GetPoint(10)->Set(1,1,-1);
	   points->GetPoint(11)->Set(1,-1,-1);

       // cube left, triangle 1
	   points->GetPoint(12)->Set(-1,1,-1);
	   points->GetPoint(13)->Set(-1,1,1);
	   points->GetPoint(14)->Set(-1,-1,-1);

	   // cube left, triangle 2
	   points->GetPoint(15)->Set(-1,1,1);
	   points->GetPoint(16)->Set(-1,-1,1);
	   points->GetPoint(17)->Set(-1,-1,-1);

       // cube top, triangle 1
	   points->GetPoint(18)->Set(-1,1,-1);
	   points->GetPoint(19)->Set(1,1,1);
	   points->GetPoint(20)->Set(-1,1,1);

	   // cube top, triangle 2
	   points->GetPoint(21)->Set(-1,1,-1);
	   points->GetPoint(22)->Set(1,1,-1);
	   points->GetPoint(23)->Set(1,1,1);

	   // cube back, triangle 1
	   points->GetPoint(24)->Set(1,1,-1);
	   points->GetPoint(25)->Set(-1,1,-1);
	   points->GetPoint(26)->Set(-1,-1,-1);

	   // cube back, triangle 2
	   points->GetPoint(27)->Set(1,1,-1);
	   points->GetPoint(28)->Set(-1,-1,-1);
	   points->GetPoint(29)->Set(1,-1,-1);

        // --- Cube colors -------
        // cube front, triangle 1
        colors->GetColor(0)->Set(1,0,0,1);
        colors->GetColor(1)->Set(1,0,0,1);
        colors->GetColor(2)->Set(1,0,0,1);

	    // cube front, triangle 2
        colors->GetColor(3)->Set(1,0,0,1);
        colors->GetColor(4)->Set(1,0,0,1);
        colors->GetColor(5)->Set(1,0,0,1);

        // cube right, triangle 1
        colors->GetColor(6)->Set(1,.75,0,1);
        colors->GetColor(7)->Set(1,.75,0,1);
        colors->GetColor(8)->Set(1,.75,0,1);

	    // cube right, triangle 2
        colors->GetColor(9)->Set(1,.75,0,1);
        colors->GetColor(10)->Set(1,.75,0,1);
        colors->GetColor(11)->Set(1,.75,0,1);

        // cube left, triangle 1
        colors->GetColor(12)->Set(1,1,0,1);
        colors->GetColor(13)->Set(1,1,0,1);
        colors->GetColor(14)->Set(1,1,0,1);

	    // cube left, triangle 2
        colors->GetColor(15)->Set(1,1,0,1);
        colors->GetColor(16)->Set(1,1,0,1);
        colors->GetColor(17)->Set(1,1,0,1);

        // cube top, triangle 1
        colors->GetColor(18)->Set(0,0,1,1);
        colors->GetColor(19)->Set(0,0,1,1);
        colors->GetColor(20)->Set(0,0,1,1);

	    // cube top, triangle 2
        colors->GetColor(21)->Set(0,0,1,1);
        colors->GetColor(22)->Set(0,0,1,1);
        colors->GetColor(23)->Set(0,0,1,1);

        // cube back, triangle 1
		colors->GetColor(24)->Set(0,1,0,1);
		colors->GetColor(25)->Set(0,1,0,1);
		colors->GetColor(26)->Set(0,1,0,1);

		// cube back, triangle 2
		colors->GetColor(27)->Set(0,1,0,1);
		colors->GetColor(28)->Set(0,1,0,1);
		colors->GetColor(29)->Set(0,1,0,1);

        meshRenderer->UpdateFromMesh();

       /* SceneObject * childSceneObject = objectManager->CreateSceneObject();
        sceneObject->AddChild(childSceneObject);
        childSceneObject->SetMesh(mesh);
        childSceneObject->SetMeshRenderer(meshRenderer);
        childSceneObject->GetTransform()->Translate(5,1,0,true);
        childSceneObject->GetTransform()->Translate(15,0,0,false);*/
    }

    void OnUpdate(Graphics * graphics)
    {
    	// cameraObject->GetTransform()->RotateAround(0,0,-12,0,1,0,.1);
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
    Graphics::Instance()->Init(640,480, (CustomGraphicsCallbacks*)(&graphicsCallbacks), "GTE Test");

    return EXIT_SUCCESS;
}

