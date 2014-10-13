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

	std::string name;
	unsigned int id;
	Matrix4x4 offsetMatrix;
	SkeletonNode * node;

	public :

	Bone();
	Bone(std::string& name);
	Bone(std::string& name, unsigned int id);
    ~Bone();

    void SetName(std::string& name);
    void SetID(unsigned int id);
    void SetOffsetMatrix(Matrix4x4& matrix);
    void SetNode(SkeletonNode * node);
};

#endif
