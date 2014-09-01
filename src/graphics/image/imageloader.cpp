#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "imageloader.h"
#include "rawimage.h"
#include "lodepng/lodepng.h"
#include "lodepng/lodepng_util.h"
#include "global/global.h"
#include "ui/debug.h"

RawImage * ImageLoader::LoadPNG(const char * fullPath)
{
	std::vector<unsigned char> image; // raw pixels
	unsigned width, height;

	//the raw pixels will be in the vector "image", 4 bytes per pixel, ordered RGBARGBA
	unsigned error = lodepng::decode(image, width, height, fullPath);

	if(error)
	{
		char msg[64];
		const char * loadErr = lodepng_error_text(error);
		sprintf(msg, "Error loading PNG: %s", loadErr);
		Debug::PrintError(msg);
		return NULL;
	}

	RawImage * raw = new RawImage(width,height);
	bool initSuccess = raw->Init();

	if(!initSuccess)
	{
		Debug::PrintError("ImageLoader::LoadPNG -> could not init raw image");
		return NULL;
	}

	for(unsigned int i=0; i < width * height * 5; i++)
	{
		raw->SetByte(i,(BYTE)image[i]);
	}

	return raw;
}
