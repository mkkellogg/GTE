#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <memory>
#include "skeleton.h"
#include "skeletonnode.h"
#include "bone.h"
#include "vertexbonemap.h"
#include "util/tree.h"
#include "global/global.h"
#include "ui/debug.h"

Skeleton::Skeleton(unsigned int boneCount)
{
	this->boneCount = boneCount;
	bones = NULL;
}

Skeleton::~Skeleton()
{
	Destroy();
}

void Skeleton::Destroy()
{
	for(unsigned int i = 0; i < vertexBoneMap.size(); i++)
	{
		VertexBoneMap * map = vertexBoneMap[i];
		SAFE_DELETE(map);
	}
	vertexBoneMap.clear();

	if(bones != NULL)
	{
		delete[] bones;
		bones = NULL;
	}
	boneNameMap.clear();

	skeleton.SetTraversalCallback([](SkeletonNode * node) -> bool
	{
		if(node != NULL)
		{
			delete node;
		}
		return true;
	});

	skeleton.Traverse();
}

unsigned int Skeleton::GetBoneCount()
{
	return boneCount;
}

bool Skeleton::Init()
{
	Destroy();

	bones = new Bone[boneCount];
	NULL_CHECK(bones,"Skeleton::Init -> Could not allocate bone array.", NULL);

	return true;
}

Tree<SkeletonNode*>::TreeNode *  Skeleton::CreateRoot(SkeletonNode * node)
{
	if(skeleton.GetRoot() == NULL)
	{
		skeleton.AddRoot(node);
	}
	return skeleton.GetRoot();
}

Tree<SkeletonNode*>::TreeNode *  Skeleton::AddChild(Tree<SkeletonNode*>::TreeNode * parent, SkeletonNode * node)
{
	NULL_CHECK(parent,"Skeleton::AddChild -> parent is NULL.", NULL);
	Tree<SkeletonNode*>::TreeNode * childNode = parent->AddChild(node);

	return childNode;
}

void Skeleton::MapBone(std::string& name, unsigned int boneIndex)
{
	boneNameMap[name] = boneIndex;
}

int Skeleton::GetBoneMapping(std::string& name)
{
	std::unordered_map<std::string,unsigned int>::const_iterator result = boneNameMap.find(name);
	if(result != boneNameMap.end())
	{
		return (*result).second;
	}

	return -1;
}

Bone* Skeleton::GetBone(unsigned int boneIndex)
{
	if(boneIndex >= boneCount)
	{
		Debug::PrintError("Skeleton::GetBone -> Index is out of range.");
		return NULL;
	}

	return bones + boneIndex;
}

void Skeleton::AddVertexBoneMap(VertexBoneMap * map)
{
	vertexBoneMap.push_back(map);
}

VertexBoneMap * Skeleton::GetVertexBoneMap(unsigned int index)
{
	if(index >= vertexBoneMap.size())
	{
		Debug::PrintError("Skeleton::GetVertexBoneMap -> Index out of range.");
		return NULL;
	}

	return vertexBoneMap[index];
}
