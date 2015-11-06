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

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include <vector>
#include <string>

namespace GTE
{
	class Atlas
	{
		friend class Graphics;

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
		std::vector<ImageDescriptor> images;

		protected:

		static Atlas* CreateGridAtlas(TextureSharedPtr texture, Real left, Real top, Real right, Real bottom, UInt32 xCount, UInt32 yCount, Bool reverseX, Bool reverseY);
		Atlas(TextureSharedPtr texture, Bool createFirstFullFrame);
		virtual ~Atlas();

		public:
		
		void AddImageDescriptor(Real left, Real top, Real right, Real bottom);
		ImageDescriptor* GetImageDescriptor(UInt32 index);
		TextureSharedPtr GetTexture();
	};
}

#endif
