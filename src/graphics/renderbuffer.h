#ifndef _RENDER_BUFFER_H
#define _RENDER_BUFFER_H

class RenderBuffer;

enum class RenderBufferType
{
	Color=1,
	Depth=2,
	Stencil=4
};

class RenderBuffer
{
    protected:

	RenderBuffer();
    ~RenderBuffer();

    public:
};

#endif
