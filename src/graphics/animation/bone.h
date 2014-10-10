#ifndef _BONE_H_
#define _BONE_H_

//forward declarations
class Transform;

#include <vector>
#include <string>
#include "object/enginetypes.h"

class Bone
{
	protected:

	std::string name;
	unsigned int id;

	public :

	virtual const Transform * GetFullTransform() const = 0;
	virtual Transform * GetLocalTransform() = 0;

	Bone();
	Bone(std::string& name);
	Bone(std::string& name, unsigned int id);
    virtual ~Bone();

    void SetName(std::string& name);
    void SetID(unsigned int id);
};

#endif
