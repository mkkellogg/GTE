#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bone.h"
#include <string>

Bone::Bone()
{
	Node = NULL;
	std::string name("");
	this->Name = name;
	this->ID = ID;
}

Bone::Bone(std::string& name) : Bone(name, (unsigned int)-1)
{

}

Bone::Bone(std::string& name, unsigned int id)
{
	this->Name = name;
	this->ID = id;
}

Bone::~Bone()
{

}

void Bone::SetTo(Bone * bone)
{
	Name = bone->Name;
	ID = bone->ID;
	OffsetMatrix.SetTo(&bone->OffsetMatrix);
	Node = bone->Node;
}



