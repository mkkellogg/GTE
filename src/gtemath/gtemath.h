#ifndef _GTEMATH_H_
#define _GTEMATH_H_

class GTEMath 
{
    public:
    
	static float InverseSquareRoot(float n);
	static float QuickInverseSquareRoot(float n);
	static float SquareRoot(float n);
    static float QuickSquareRoot(float n);
    static float Round(float n);
    static float Cos(float n);
    static float ACos(float n);
    static float Sin(float n);
    static float Abs(float n);
    static float Min(float a, float b);
    static float Max(float a, float b);
    static float Lerp(float a, float b, float t);
};

#endif
