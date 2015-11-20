/*
* class: Atlas
*
* author: Mark Kellogg
*
* Utility class that is used for partitionin a texture into multiple frames
* UV coordinates that cover only a portion of the entire texture.
*
*/

#ifndef _GTE_ATLAS_H_
#define _GTE_ATLAS_H_

#include "engine.h"
#include "object/engineobject.h"

#include <vector>
#include <string>

namespace GTE
{
	class Atlas
	{
		friend class Graphics;
		friend class EngineObjectManager;

		public:

		class ImageDescriptor
		{
			public:

			Real Left;
			Real Top;
			Real Right;
			Real Bottom;

			ImageDescriptor(Real left, Real top, Real right, Real bottom)
			{
				Left = left;
				Top = top;
				Right = right;
				Bottom = bottom;
			}
		};

		private:

		TextureSharedPtr texture;
		UInt32 imageCount; 
		std::vector<ImageDescriptor> imageDescriptors;

		protected:

		static Atlas* CreateGridAtlas(TextureRef texture, Real left, Real top, Real right, Real bottom, UInt32 xCount, UInt32 yCount, Bool reverseX, Bool reverseY);
		Atlas(TextureRef texture, Bool createFirstFullFrame);
		virtual ~Atlas();

		public:
		
		void AddImageDescriptor(Real left, Real top, Real right, Real bottom);
		ImageDescriptor* GetImageDescriptor(UInt32 index);
		TextureRef GetTexture();
	};
}

#endif
