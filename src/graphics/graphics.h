#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <GL/glew.h>
#include <GL/glut.h>

class GraphicsCallbacks
{
        public:

        virtual void OnInit() = 0;
};

class Graphics
{
    protected:

    Graphics();

    public :

    ~Graphics();
    virtual void Init(int windowWidth, int windowHeight, GraphicsCallbacks * callbacks) = 0;
};

#endif
