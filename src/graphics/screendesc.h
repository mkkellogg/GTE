#ifndef _GTE_SCREENDESC_H_
#define _GTE_SCREENDESC_H_

// forward declarations

class ScreenDescriptor
{
	int screenWidth;
	int screenHeight;

    public:

	ScreenDescriptor();
	ScreenDescriptor(int screenWidth, int screenHeight);
    ~ScreenDescriptor();

    int GetScreenWidth();
    int getScreenHeight();
};

#endif
