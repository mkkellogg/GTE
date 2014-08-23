#ifndef _MESH3D_H_
#define _MESH3D_H_

//forward declarations
class Point3;
class Vector3;
class color4;
class uv2;

class Point3Array;
class Vector3Array;
class Color4Array;
class UV2Array;

#include "object/sceneobjectcomponent.h"
#include "graphics/attributes.h"

class Mesh3D : public SceneObjectComponent
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

