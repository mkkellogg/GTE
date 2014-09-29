#ifndef _BASEVECTOR4ARRAY_H_
#define _BASEVECTOR4ARRAY_H_

// forward declarations
class BaseVector4;
class BaseVector4Factory;

class BaseVector4Array
{
	protected:

	unsigned int count;
	float * data;
	BaseVector4 ** objects;
	BaseVector4Factory * baseFactory;

	void Destroy();

    public:

	BaseVector4Array(BaseVector4Factory * factory);
    virtual ~BaseVector4Array();

    const float * GetDataPtr() const;
    bool Init(unsigned int count);
    unsigned int GetCount();
    bool CopyTo(BaseVector4Array * dest) const;
};

#endif
