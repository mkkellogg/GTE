#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "basevector4.h"
#include "global/global.h"
#include "ui/debug.h"

/*
 * Initialize - Store [x], [y], [z], and [w] as the base data.
 *
 * If [target] is not NULL, then use it as the storage array.
 *
 * If [permAttach] is true, then [target] cannot be NULL, as it will be used as the permanent
 * backing storage array. If [permAttach] is false, then this base vector can attach and
 * re-attach to various different backing storage arrays.
 */
void BaseVector4::Init(float x, float y, float z, float w, float *target, bool permAttach)
{
	//data = baseData = NULL;
	data = baseData;

	if(target == NULL && permAttach == true)
	{
		Debug::PrintError("BaseVector4::Init -> permAttach is true, but target is NULL!");
	}

	if(permAttach)
	{
		canDetach = false;
	}
	else
	{
		//data = baseData = new float[4];
		data = baseData;
		canDetach = true;
	}

	if(target != NULL)
	{
		data = target;
		attached = true;
	}
	else
	{
		attached = false;
	}
    Set(x,y,z,w);
}

/*
 * Default constructor
 */
BaseVector4::BaseVector4()
{
    Init(0,0,0,0, NULL, false);
}

/*
 * Constructor will alternate backing storage
 */
BaseVector4::BaseVector4(bool permAttached, float * target)
{
	Init(0,0,0,0,target, true);
}

/*
 * Constructor with initialization values
 */
BaseVector4::BaseVector4(float x, float y, float z, float w)
{
    Init(x,y,z,w, NULL, false);
}

/*
 * Copy constructor
 */
BaseVector4::BaseVector4(const BaseVector4 * baseVector)
{
    Init(baseVector->data[0], baseVector->data[1], baseVector->data[2], baseVector->data[3], NULL, false);
}

/*
 * Clean up
 */
BaseVector4::~BaseVector4()
{   
	//SAFE_DELETE(baseData);

    if(!attached)data = NULL;
}


/*
 * Set this base vector's data
 */
void BaseVector4::Set(float x, float y, float z, float w)
{
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

/*
 * Set this base vector's data to be equal to [baseVector]
 */
void BaseVector4::SetTo(BaseVector4 * baseVector)
{
	ASSERT_RTRN(baseVector != NULL, "BaseVector4::SetTo -> NULL baseVector passed.");

	data[0] = baseVector->data[0];
	data[1] = baseVector->data[1];
	data[2] = baseVector->data[2];
	data[3] = baseVector->data[3];
}

/*
 * Copy this base vector's data into [baseVector]
 */
void BaseVector4::Get(BaseVector4 * baseVector) const
{
	ASSERT_RTRN(baseVector != NULL, "BaseVector4::Get -> NULL baseVector passed.");
	memcpy(baseVector->data, data, sizeof(float) * 4);
}

/*
 * Get a pointer the backing data storage
 */
float * BaseVector4::GetDataPtr()
{
    return data;
}

/*
 * Attach the base vector to a new backing storage array [data]
 */
void BaseVector4::AttachTo(float * data)
{
    this->data = data;
    attached = true;
}

/*
 * Detach from the current backing storage array
 */
void BaseVector4::Detach()
{
	if(canDetach)
	{
		this->data = baseData;
		attached = false;
	}
}





