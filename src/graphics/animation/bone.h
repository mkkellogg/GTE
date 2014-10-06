#ifndef _BONE_H_
#define _BONE_H_

//forward declarations
class Transform;

#include <vector>
#include <string>

class Bone
{
	Transform * target;
	std::string name;
	unsigned int id;

	public :

	Bone();
	Bone(std::string& name);
	Bone(std::string& name, unsigned int id);
    ~Bone();

    void SetName(std::string& name);
    void SetID(unsigned int id);
};

#endif
