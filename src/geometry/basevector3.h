#ifndef _BASEVECTOR3_H_
#define _BASEVECTOR3_H_

class BaseVector3 
{
    protected:

    void Init(float x, float y, float z, float w);

    float * data;
    bool deleteData;

    public:
    
    float &x;
    float &y;
    float &z;
    float &w;

    BaseVector3();
    BaseVector3(float x, float y, float z, float w);
    BaseVector3(BaseVector3 * baseVector);
    BaseVector3(float * copyData);
    virtual ~BaseVector3();

    BaseVector3 & operator= (const BaseVector3 & source);
    float * GetDataPtr();
    void Set(float x, float y, float z, float w);
    void Set(float x, float y, float z);
    void Get(BaseVector3 * baseVector);
};

#endif
