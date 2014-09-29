#ifndef _BONE_H_
#define _BONE_H_

//forward declarations
class Transform;

#include <vector>

class Bone
{
	Transform * target;

	public :

	Bone();
    ~Bone();
};

#endif
