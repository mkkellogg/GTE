
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
    Shader * basicShader;

    public : 

    CustomGraphicsCallbacks()
    {
        basicShader = NULL;
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
        //basicShader = graphics->CreateShader("resources/basic.vertex.shader","resources/basic.fragment.shader");
        //basicShader->Load();

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

        SceneObject * cameraObject = objectManager->CreateSceneObject();
        Camera * camera = objectManager->CreateCamera();
        camera->AddClearBuffer(RenderBufferType::Color);
        camera->AddClearBuffer(RenderBufferType::Depth);
        cameraObject->SetCamera(camera);

        SceneObject * sceneObject = objectManager->CreateSceneObject();

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
        points->GetPoint(0)->Set(-1,1,-11);
        points->GetPoint(1)->Set(1,1,-11);
        points->GetPoint(2)->Set(-1,-1,-11);

        // cube front, triangle 2
        points->GetPoint(3)->Set(1,1,-11);
        points->GetPoint(4)->Set(1,-1,-11);
        points->GetPoint(5)->Set(-1,-1,-11);

        // --- Cube colors -------
        // cube front, triangle 1
        colors->GetColor(0)->Set(1,0,0,1);
        colors->GetColor(1)->Set(1,0,0,1);
        colors->GetColor(2)->Set(1,0,0,1);

	    // cube front, triangle 2
        colors->GetColor(3)->Set(1,0,0,1);
        colors->GetColor(4)->Set(1,0,0,1);
        colors->GetColor(5)->Set(1,0,0,1);

        meshRenderer->UpdateFromMesh();
}

    void OnUpdate(Graphics * graphics)
    {

    }

    void OnQuit(Graphics * graphics)
    {   
        if(basicShader != NULL)
        {
            delete basicShader;
            basicShader = NULL;
        }
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

