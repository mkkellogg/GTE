#ifndef _BASEVECTOR4_H_
#define _BASEVECTOR4_H_

class BaseVector4
{
    protected:

    void Init(float x, float y, float z, float w, float *target, bool permAttach);

    float * data;
    float * baseData;
    bool attached;
    bool canDetach;

    public:

    BaseVector4();
    BaseVector4(bool permAttached, float * target);
    BaseVector4(float x, float y, float z, float w);
    BaseVector4(const BaseVector4 * baseVector);
    BaseVector4(const float * copyData);
    virtual ~BaseVector4();

    BaseVector4 & operator= (const BaseVector4 & source);
    float * GetDataPtr() const;
    void Set(float x, float y, float z, float w);
    void Get(BaseVector4 * baseVector);

    virtual void AttachTo(float * data);
    virtual void Detach();
};

#endif
