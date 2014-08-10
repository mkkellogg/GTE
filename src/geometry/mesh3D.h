#ifndef _MESH3D_H_
#define _MESH3D_H_

class Mesh3D
{
    float * positions;
    float * normals;
    float * uv1;
    float * uv2;

    public:

    Mesh3D();
    virtual ~Mesh3D();
};

#endif

