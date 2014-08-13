#ifndef _BASEVECTOR3_H_
#define _BASEVECTOR3_H_

class BaseVector4
{
    protected:

    void Init(float x, float y, float z, float w);

    float * data;
    float * baseData;
    bool attached;

    public:

    BaseVector4();
    BaseVector4(float x, float y, float z, float w);
    BaseVector4(const BaseVector4 * baseVector);
    BaseVector4(const float * copyData);
    virtual ~BaseVector4();

    BaseVector4 & operator= (const BaseVector4 & source);
    float * GetDataPtr() const;
    void Set(float x, float y, float z, float w);
    void Set(float x, float y, float z);
    void Get(BaseVector4 * baseVector);

    virtual void AttachTo(float * data);
    virtual void Detach();
};

#endif