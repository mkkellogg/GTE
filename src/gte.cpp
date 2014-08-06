
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "graphics/shader/shader.h"
#include "ui/debug.h"
#include "gte.h"

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(640, 480);

    if(!glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
    {
       exit(1);
    }

    (void)glutCreateWindow("GLUT Program");

    glewExperimental = GL_TRUE; 
    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        printf("Ready for OpenGL 2.0\n");
    else {	
        printf("OpenGL 2.0 not supported\n");
        exit(1);
    }

    Shader basicShader("resources/basic.vertex.shader","resources/basic.fragment.shader");
    basicShader.Load();

    glutMainLoop(); 

    return EXIT_SUCCESS;
}
