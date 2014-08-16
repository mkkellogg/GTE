#ifndef _MESH3D_H_
#define _MESH3D_H_

#include "geometry/point3.h"
#include "geometry/vector3.h"
#include "graphics/color4.h"
#include "graphics/uv2.h"

#include "geometry/point3array.h"
#include "geometry/vector3array.h"
#include "graphics/color4array.h"
#include "graphics/uv2array.h"

enum class AttributeType
{
    Position=1,
    Normal=2,
    Color=4,
    UV1=8,
    UV2=16
};

class Mesh3D
{
	int typeMask;

    Point3Array * positions;
    Vector3Array * normals;
    Color4Array * colors;
    UV2Array * uvs1;
    UV2Array * uvs2;

    void Destroy();

    public:

    Mesh3D(int type);
    virtual ~Mesh3D();

    bool Init(int attributeCount);

    Point3Array * GetPostions();
    Vector3Array * GetNormals();
    Color4Array * GetColors();
    UV2Array * GetUVs1();
    UV2Array * GetUVs2();
};

#endif

