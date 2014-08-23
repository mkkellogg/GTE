#ifndef _BASEVECTOR2ARRAY_H_
#define _BASEVECTOR2ARRAY_H_

// forward declarations
class BaseVector2;
class BaseVector2Factory;

class BaseVector2Array
{
	protected:

	int count;
	float * data;
	BaseVector2 ** objects;
	BaseVector2Factory * baseFactory;

	void Destroy();

    public:

	BaseVector2Array(BaseVector2Factory * factory);
    virtual ~BaseVector2Array();

    const float * GetDataPtr() const;
    bool Init(int count);
};

#endif
