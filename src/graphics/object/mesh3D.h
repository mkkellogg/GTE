#ifndef _MESH3D_H_
#define _MESH3D_H_

//forward declarations
class Point3;
class Vector3;
class color4;
class UV2;
class Point3Array;
class Vector3Array;
class Color4Array;
class UV2Array;
class EngineObjectManager;
class Mesh3DRenderer;

#include "object/sceneobjectcomponent.h"
#include "graphics/stdattributes.h"

class Mesh3D : public SceneObjectComponent
{
	friend EngineObjectManager;

	Mesh3DRenderer * renderer;
	StandardAttributeSet attributeSet;
	unsigned int vertexCount;

    Point3Array * positions;
    Vector3Array * normals;
    Color4Array * colors;
    UV2Array * uvsTexture0;
    UV2Array * uvsTexture1;

    void Destroy();

	protected :

    Mesh3D();
    Mesh3D(StandardAttributeSet attributes);
    virtual ~Mesh3D();

	public:

    void SetRenderer(Mesh3DRenderer * render);
    bool Init(unsigned int vertexCount);
    unsigned int GetVertexCount();
    StandardAttributeSet GetAttributeSet();
    void CalculateNormals(float smoothingThreshhold);

    Point3Array * GetPostions();
    Vector3Array * GetNormals();
    Color4Array * GetColors();
    UV2Array * GetUVsTexture0();
    UV2Array * GetUVsTexture1();
};

#endif

