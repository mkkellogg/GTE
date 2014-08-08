#ifndef _BASEVECTOR3_H_
#define _BASEVECTOR3_H_

class BaseVector3 
{
    protected:

    void Init(float x, float y, float z);

    float * data;
    bool deleteData;

    public:
    
    float &x;
    float &y;
    float &z;
    float &w;

    BaseVector3();
    BaseVector3(float x, float y, float z);
    BaseVector3(BaseVector3 * baseVector);
    virtual ~BaseVector3();

    const float * GetData();
    void Set(float x, float y, float z);
    void Get(BaseVector3 * baseVector);

};

#endif
