#ifndef _BONE_H_
#define _BONE_H_

//forward declarations
class Transform;
class SkeletonNode;

#include <vector>
#include <string>
#include "object/enginetypes.h"
#include "geometry/matrix4x4.h"

class Bone
{
	protected:

	public :

	Bone();
	Bone(std::string& name);
	Bone(std::string& name, unsigned int id);
    ~Bone();

    std::string Name;
    unsigned int ID;
    Matrix4x4 OffsetMatrix;
    Matrix4x4 TempFullMatrix;
    SkeletonNode * Node;

    void SetTo(Bone * bone);

};

#endif
