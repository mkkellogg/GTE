/*********************************************
*
* class: Bone
*
* author: Mark Kellogg
*
* This class represents a single bone in a Skeleton object.
*
***********************************************/

#ifndef _GTE_BONE_H_
#define _GTE_BONE_H_

//forward declarations
class Transform;
class SkeletonNode;

#include <vector>
#include <string>
#include "object/enginetypes.h"
#include "geometry/matrix4x4.h"

class Bone
{
	public :

	Bone();
	Bone(std::string& name);
	Bone(std::string& name, unsigned int id);
    ~Bone();

    // the bone's (unique) name. Used for linking this Bone object
    // to its corresponding node in a Skeleton or scene object hierarchy.
    std::string Name;
    // this bone's unique ID
    unsigned int ID;
    // this matrix converts the bone (and attached vertices) to bone space
    Matrix4x4 OffsetMatrix;
    Matrix4x4 TempFullMatrix;
    // when this bone is part of a Skeleton object, [Node] points to this bone's
    // corresponding SkeletonNode object in that Skeleton object.
    SkeletonNode * Node;

    void SetTo(Bone * bone);

};

#endif
