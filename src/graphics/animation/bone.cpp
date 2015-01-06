#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bone.h"
#include <string>

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
Bone::Bone(std::string& name) : Bone(name, (unsigned int)-1)
{

}

/*
 * Fully parameterized constructor.
 */
Bone::Bone(std::string& name, unsigned int id)
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
void Bone::SetTo(Bone * bone)
{
	Name = bone->Name;
	ID = bone->ID;
	OffsetMatrix.SetTo(bone->OffsetMatrix);
	Node = bone->Node;
}



