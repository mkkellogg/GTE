
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "graphics/graphics.h"
#include "graphics/shader/shader.h"
#include "ui/debug.h"
#include "gte.h"

#include "geometry/point3.h"

class CustomGraphicsCallbacks : public GraphicsCallbacks
{
    Shader * basicShader;

    public : 

    CustomGraphicsCallbacks()
    {
        basicShader = NULL;
    }

    void OnInit(Graphics * graphics)
    {
        ShaderManager * shaderManager = graphics->GetShaderManager();
        basicShader = shaderManager->CreateShader("resources/basic.vertex.shader","resources/basic.fragment.shader");
        basicShader->Load();

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
    }

    void OnQuit(Graphics * graphics)
    {   
        if(basicShader != NULL)
        {
            delete basicShader;
            basicShader = NULL;
        }
    }
};

int main(int argc, char** argv)
{
    CustomGraphicsCallbacks graphicsCallbacks;
    Graphics::Instance()->Init(640,480, (CustomGraphicsCallbacks*)(&graphicsCallbacks));   

    return EXIT_SUCCESS;
}

