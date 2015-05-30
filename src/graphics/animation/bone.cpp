#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bone.h"
#include <string>

namespace GTE
{
	/*
	* Default constructor.
	*/
	Bone::Bone()
	{
		Node = NULL;
		this->Name = std::string("");
		this->ID = -1;
	}

	/*
	 * Parameterized constructor only with [name] parameter.
	 */
	Bone::Bone(const std::string& name) : Bone(name, (unsigned int)-1)
	{

	}

	/*
	 * Fully parameterized constructor.
	 */
	Bone::Bone(const std::string& name, unsigned int id)
	{
		this->Name = name;
		this->ID = id;
		Node = NULL;
	}

	/*
	 * Destructor
	 */
	Bone::~Bone()
	{

	}

	/*
	 * Copy member data from [bone].
	 */
	void Bone::SetTo(const Bone * bone)
	{
		Name = bone->Name;
		ID = bone->ID;
		OffsetMatrix.SetTo(bone->OffsetMatrix);
		Node = bone->Node;
	}

}


