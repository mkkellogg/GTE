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

#include "graphics/attributes.h"

class Mesh3D
{
	AttributeSet attributeSet;
	int vertexCount;

    Point3Array * positions;
    Vector3Array * normals;
    Color4Array * colors;
    UV2Array * uvs1;
    UV2Array * uvs2;

    void Destroy();

    public:

    Mesh3D(AttributeSet attributes);
    virtual ~Mesh3D();

    bool Init(int vertexCount);
    int GetVertexCount();
    AttributeSet GetAttributeSet();

    Point3Array * GetPostions();
    Vector3Array * GetNormals();
    Color4Array * GetColors();
    UV2Array * GetUVs1();
    UV2Array * GetUVs2();
};

#endif

