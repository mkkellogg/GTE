#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bone.h"
#include <string>

Bone::Bone(std::string& name) : Bone(name, (unsigned int)-1)
{

}

Bone::Bone(std::string& name, unsigned int id)
{
	target = NULL;
	this->name = name;
	this->id = id;
}

Bone::~Bone()
{

}
