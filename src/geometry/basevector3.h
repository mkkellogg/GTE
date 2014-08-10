#ifndef _BASEVECTOR3_H_
#define _BASEVECTOR3_H_

class BaseVector3 
{
    protected:

    void Init(float x, float y, float z, float w);
    void UpdateComponentPointers();

    float * data;
    float * baseData;
    bool attached;

    public:
    
    float &x;
    float &y;
    float &z;
    float &w;

    BaseVector3();
    BaseVector3(float x, float y, float z, float w);
    BaseVector3(const BaseVector3 * baseVector);
    BaseVector3(const float * copyData);
    virtual ~BaseVector3();

    BaseVector3 & operator= (const BaseVector3 & source);
    float * GetDataPtr();
    void Set(float x, float y, float z, float w);
    void Set(float x, float y, float z);
    void Get(BaseVector3 * baseVector);

    void AttachTo(float * data);
    void Detach();
};

#endif
