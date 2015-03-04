
 /*
 * Graphics properties and definitions.
 *
 * author: Mark Kellogg
 *
 * Several types that are used by the core graphics components.
 */

#ifndef _GRAPHICSATTR_H_
#define _GRAPHICSATTR_H_

enum class BlendingProperty
{
	One,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha,
	Zero
};

enum class ColorChannel
{
	Red = 0,
	Green = 1,
	Blue = 2,
	Alpha = 3
};

enum class RenderMode
{
	Standard = 1,
	StandardWithShadowVolumeTest = 2,
	ShadowVolumeRender = 3,
	DepthOnly = 4,
	None = 0
};

enum class SSAORenderMode
{
	Standard = 0,
	Outline = 1
};

/*
 * An instance of GraphicsAttributes is used to define the fundamental
 * attributes of the graphics system.
 */
class GraphicsAttributes
{
	public:

	unsigned int WindowWidth;
	unsigned int WindowHeight;
	std::string WindowTitle;
	bool SSAOEnabled;
	SSAORenderMode SSAOMode;

	GraphicsAttributes()
	{
		WindowWidth = 640;
		WindowHeight = 480;
		WindowTitle = std::string("GTE window");
		SSAOEnabled = true;
		SSAOMode = SSAORenderMode::Standard;
	}
};


#endif
