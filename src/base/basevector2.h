#ifndef _GTE_BASEVECTOR2_H_
#define _GTE_BASEVECTOR2_H_

class BaseVector2
{
    protected:

    void Init(float *target, bool permAttach);

    float * data;
    float baseData[2];
    bool attached;
    bool canDetach;

    public:

    BaseVector2();
    BaseVector2(bool permAttached, float * target);
    BaseVector2(float x, float y);
    BaseVector2(const BaseVector2& baseVector);
    BaseVector2(const float * copyData);
    virtual ~BaseVector2();

    BaseVector2 & operator= (const BaseVector2 & source);
    float * GetDataPtr() const;
    void Set(float x, float y);
    void Get(BaseVector2& baseVector);

    virtual void AttachTo(float * data);
    virtual void Detach();
};

#endif
