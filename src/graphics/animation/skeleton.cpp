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

	skeleton.SetTraversalCallback([](Tree<SkeletonNode *>::TreeNode * node) -> bool
	{
		if(node != NULL && node->Data != NULL)
		{
			delete node->Data;
		}
		return true;
	});

	skeleton.Traverse();
}

unsigned int Skeleton::GetBoneCount()
{
	return boneCount;
}

unsigned int Skeleton::GetNodeCount()
{
	return nodeList.size();
}

bool Skeleton::Init()
{
	Destroy();

	bones = new Bone[boneCount];
	NULL_CHECK(bones,"Skeleton::Init -> Could not allocate bone array.", false);

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

void Skeleton::MapNode(std::string& name, unsigned int nodeIndex)
{
	nodeNameMap[name] = nodeIndex;
}

int Skeleton::GetNodeMapping(std::string& name)
{
	std::unordered_map<std::string,unsigned int>::const_iterator result = nodeNameMap.find(name);
	if(result != nodeNameMap.end())
	{
		return (*result).second;
	}

	return -1;
}

SkeletonNode * Skeleton::GetNodeFromList(unsigned int nodeIndex)
{
	if(nodeIndex >= GetNodeCount())
	{
		Debug::PrintError("Skeleton::GetNodeFromList -> Index is out of range.");
		return NULL;
	}

	return nodeList[nodeIndex];
}

void Skeleton::AddNodeToList(SkeletonNode * node)
{
	nodeList.push_back(node);
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

Skeleton * Skeleton::FullClone()
{
	Skeleton * newSkeleton = new Skeleton(boneCount);
	NULL_CHECK(newSkeleton,"Skeleton::FullClone -> could not allocate skeleton",NULL);

	bool initSuccess = newSkeleton->Init();
	if(!initSuccess)
	{
		delete newSkeleton;
		return NULL;
	}

	for(unsigned int i = 0; i < boneCount; i++)
	{
		newSkeleton->GetBone(i)->SetTo(GetBone(i));
	}

	for(unsigned int i = 0; i < vertexBoneMap.size(); i++)
	{
		VertexBoneMap * ptr = vertexBoneMap[i];
		VertexBoneMap * clone = NULL;

		if(ptr != NULL)
		{
			clone = ptr->FullClone();
			if(clone == NULL)
			{
				Debug::PrintError("Skeleton::FullClone -> Could not clone vertex bone map.");
				delete newSkeleton;
				return NULL;
			}
		}

		newSkeleton->vertexBoneMap.push_back(clone);
	}

	newSkeleton->boneNameMap = boneNameMap;

	Tree<SkeletonNode *>::TreeNode * root = skeleton.GetRoot();

	if(root != NULL)
	{
		SkeletonNode * rootClone = NULL;
		if(root->Data != NULL)
		{
			rootClone = root->Data->FullClone();
			if(rootClone == NULL)
			{
				Debug::PrintError("Skeleton::FullClone -> Could not clone root node.");
				delete newSkeleton;
				return NULL;
			}
		}

		Tree<SkeletonNode *>::TreeNode * newRoot = newSkeleton->CreateRoot(rootClone);
		if(newRoot == NULL)
		{
			Debug::PrintError("Skeleton::FullClone -> Could not create root node.");
			delete newSkeleton;
			return NULL;
		}



		std::unordered_map<Tree<SkeletonNode *>::TreeNode *, Tree<SkeletonNode *>::TreeNode *> newNodeMap;
		newNodeMap[root] = newRoot;



		bool allocateTreeSuccess = true;
		skeleton.SetTraversalCallback([&allocateTreeSuccess, &newNodeMap](Tree<SkeletonNode *>::TreeNode * node) -> bool
		{
			Tree<SkeletonNode *>::TreeNode * newNode = new Tree<SkeletonNode *>::TreeNode();
			if(newNode == NULL)
			{
				Debug::PrintError("Skeleton::FullClone -> Could not allocate new node.");
				allocateTreeSuccess = false;
				return NULL;
			}
			newNodeMap[node] = newNode;

			return true;
		});

		skeleton.Traverse();

		if(!allocateTreeSuccess)
		{
			Debug::PrintError("Skeleton::FullClone -> Could not allocate new nodes for skeleton clone.");
			delete newSkeleton;
			return NULL;
		}

		newSkeleton->nodeNameMap = nodeNameMap;
		newSkeleton->nodeList.resize(nodeList.size());

		bool cloneTreeSuccess = true;
		Skeleton * thisSkeleton = this;
		skeleton.SetTraversalCallback([&cloneTreeSuccess, &newNodeMap, newSkeleton, thisSkeleton](Tree<SkeletonNode *>::TreeNode * node) -> bool
		{
			SkeletonNode * clonedSkeletonNode = NULL;
			if(node != NULL && node->Data != NULL)
			{
				clonedSkeletonNode = node->Data->FullClone();
				if(clonedSkeletonNode == NULL)
				{
					Debug::PrintError("Skeleton::FullClone -> Could not clone node in skeletal tree.");
					cloneTreeSuccess = false;
					return NULL;
				}
			}

			Tree<SkeletonNode *>::TreeNode * clonedTreeNode = newNodeMap[node];
			clonedTreeNode->Data = clonedSkeletonNode;

			int targetListIndex = thisSkeleton->GetNodeMapping(clonedSkeletonNode->Name);
			if(targetListIndex >= 0)newSkeleton->nodeList[targetListIndex] = clonedSkeletonNode;

			Tree<SkeletonNode *>::TreeNode * originalParent = clonedTreeNode->GetParent();
			if(originalParent != NULL)
			{
				Tree<SkeletonNode *>::TreeNode * clonedParentNode = newNodeMap[originalParent];
				clonedParentNode->AddChild(clonedTreeNode);
			}

			int boneIndex = node->Data->BoneIndex;
			if(boneIndex >= 0)
			{
				newSkeleton->GetBone(boneIndex)->Node = clonedSkeletonNode;
			}

			return true;
		});

		skeleton.Traverse();

		if(!cloneTreeSuccess)
		{
			Debug::PrintError("Skeleton::FullClone -> Could not clone skeletal tree.");
			delete newSkeleton;
			return NULL;
		}
	}

	return newSkeleton;
}
