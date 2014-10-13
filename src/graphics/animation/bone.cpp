#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bone.h"
#include <string>

Bone::Bone()
{
	node = NULL;
	std::string name("");
	SetName(name);
	SetID((unsigned int)-1);
}

Bone::Bone(std::string& name) : Bone(name, (unsigned int)-1)
{

}

Bone::Bone(std::string& name, unsigned int id)
{
	SetName(name);
	SetID(id);
}

Bone::~Bone()
{

}

void Bone::SetName(std::string& name)
{
	this->name = name;
}

void Bone::SetID(unsigned int id)
{
	this->id = id;
}

void Bone::SetOffsetMatrix(Matrix4x4& matrix)
{
	this->offsetMatrix.SetTo(&matrix);
}

void Bone::SetNode(SkeletonNode * node)
{
	this->node= node;
}
