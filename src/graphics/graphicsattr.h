
 /*
 * Graphics properties and definitions.
 *
 * author: Mark Kellogg
 *
 * Several types that are used by the core graphics components.
 */

#ifndef _GTE_GRAPHICSATTR_H_
#define _GTE_GRAPHICSATTR_H_

namespace GTE
{
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

	enum class FaceCullingMode
	{
		Back = 0,
		Front = 1
	};

	enum class AntialiasingMethod
	{
		None = 0,
		MSAAx2 = 1,
		MSAAx4 = 2,
		MSAAx8 = 3,
		MSAAx16 = 4
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
		bool WaitForVSync;
		AntialiasingMethod AAMethod;

		GraphicsAttributes()
		{
			WindowWidth = 640;
			WindowHeight = 480;
			WindowTitle = std::string("GTE window");
			WaitForVSync = false;
			AAMethod = AntialiasingMethod::MSAAx2;
		}

		static unsigned int GetMSAASamples(AntialiasingMethod method)
		{
			switch (method)
			{
			case AntialiasingMethod::MSAAx2:
				return 2;
				break;
			case AntialiasingMethod::MSAAx4:
				return 4;
				break;
			case AntialiasingMethod::MSAAx8:
				return 8;
				break;
			case AntialiasingMethod::MSAAx16:
				return 16;
				break;
			default:
				return 0;
				break;
			}
		}

		static bool IsMSAA(AntialiasingMethod method)
		{
			switch (method)
			{
			case AntialiasingMethod::MSAAx2:
			case AntialiasingMethod::MSAAx4:
			case AntialiasingMethod::MSAAx8:
			case AntialiasingMethod::MSAAx16:
				return true;
				break;
			default:
				return false;
				break;
			}
		}
	};
}

#endif
