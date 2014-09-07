/*
 * class: BaseVector4
 *
 * author: Mark Kellogg
 *
 * Forms the basis of several vertex attribute classes (Point3, Vector3, Color4, ...) and contains
 * functionality that is shared by all of them.
 */

#ifndef _BASEVECTOR4_H_
#define _BASEVECTOR4_H_

class BaseVector4
{
    protected:

    void Init(float x, float y, float z, float w, float *target, bool permAttach);

    float * data;
   // float * baseData;
    float baseData[4];
    bool attached;
    bool canDetach;

    public:

    BaseVector4();
    BaseVector4(bool permAttached, float * target);
    BaseVector4(float x, float y, float z, float w);
    BaseVector4(const BaseVector4 * baseVector);
    BaseVector4(const float * copyData);
    virtual ~BaseVector4();

    float * GetDataPtr();
    virtual void Set(float x, float y, float z, float w);
    void Get(BaseVector4 * baseVector) const;

    virtual void AttachTo(float * data);
    virtual void Detach();
};

#endif
