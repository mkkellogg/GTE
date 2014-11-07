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

/*
 * Only constructor.
 */
Skeleton::Skeleton(unsigned int boneCount)
{
	this->boneCount = boneCount;
	bones = NULL;
}
/*
 * Destructor.
 */
Skeleton::~Skeleton()
{
	Destroy();
}

/*
 * Destroy the skeleton. Delete all VertexBoneMap objects, all Bone objects, and
 * all SkeletonNode objects.
 */
void Skeleton::Destroy()
{
	// loop through each VertexBoneMap and delete
	for(unsigned int i = 0; i < vertexBoneMaps.size(); i++)
	{
		VertexBoneMap * map = vertexBoneMaps[i];
		SAFE_DELETE(map);
	}
	vertexBoneMaps.clear();

	// delete all Bone objects
	if(bones != NULL)
	{
		delete[] bones;
		bones = NULL;
	}
	boneNameMap.clear();

	// delete all SkeletonNode objects by traversing the node hierarchy and
	// using a visitor to invoke the callback below, which performsm the delete.
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
/*
 * Get the number of bones in this skeleton.
 */
unsigned int Skeleton::GetBoneCount()
{
	return boneCount;
}

/*
 * Get the number of nodes in this skeleton.
 */
unsigned int Skeleton::GetNodeCount()
{
	return nodeList.size();
}

/*
 * Initialize the Skeleton object. This MUST be called before the Skeleton object can
 * be used in any way. It will return true if initialization success; false otherwise.
 */
bool Skeleton::Init()
{
	// destroy existing data (if there is any)
	Destroy();

	bones = new Bone[boneCount];
	ASSERT(bones != NULL,"Skeleton::Init -> Could not allocate bone array.", false);

	return true;
}

/*
 * Create a root node for this Skeleton object. The root node is a dummy node and only serves
 * to connect all other nodes to a single root ancestor. It does not have a bone associated with
 * it and does not have a target. It has no transformation associated with it.
 */
Tree<SkeletonNode*>::TreeNode *  Skeleton::CreateRoot(SkeletonNode * node)
{
	if(skeleton.GetRoot() == NULL)
	{
		skeleton.AddRoot(node);
	}
	return skeleton.GetRoot();
}

/*
 * Connect a child node [child] to a parent node [parent].
 */
Tree<SkeletonNode*>::TreeNode *  Skeleton::AddChild(Tree<SkeletonNode*>::TreeNode * parent, SkeletonNode * node)
{
	ASSERT(parent != NULL,"Skeleton::AddChild -> parent is NULL.", NULL);
	Tree<SkeletonNode*>::TreeNode * childNode = parent->AddChild(node);

	return childNode;
}

/*
 * Set the mapping from a bone name to its index in [boneNameMap]
 */
void Skeleton::MapBone(std::string& name, unsigned int boneIndex)
{
	boneNameMap[name] = boneIndex;
}

/*
 * Get the index in [boneNameMap] to which [name] corresponds. If no valid mapping
 * is found, -1 is returned.
 */
int Skeleton::GetBoneMapping(std::string& name)
{
	std::unordered_map<std::string,unsigned int>::const_iterator result = boneNameMap.find(name);
	if(result != boneNameMap.end())
	{
		return (*result).second;
	}

	return -1;
}

/*
 * Get the Bone object stored at [index] in [bones].
 */
Bone* Skeleton::GetBone(unsigned int boneIndex)
{
	if(boneIndex >= boneCount)
	{
		Debug::PrintError("Skeleton::GetBone -> Index is out of range.");
		return NULL;
	}

	return bones + boneIndex;
}

/*
 * Set the mapping from a node name to its index in [nodeNameMap]
 */
void Skeleton::MapNode(std::string& name, unsigned int nodeIndex)
{
	nodeNameMap[name] = nodeIndex;
}

/*
 * Get the index in [nodeName] to which [name] corresponds. If no valid mapping
 * is found, -1 is returned.
 */
int Skeleton::GetNodeMapping(std::string& name)
{
	std::unordered_map<std::string,unsigned int>::const_iterator result = nodeNameMap.find(name);
	if(result != nodeNameMap.end())
	{
		return (*result).second;
	}

	return -1;
}

/*
 * Get the SkeletonNode object stored at [index] in [nodeList].
 */
SkeletonNode * Skeleton::GetNodeFromList(unsigned int nodeIndex)
{
	if(nodeIndex >= GetNodeCount())
	{
		Debug::PrintError("Skeleton::GetNodeFromList -> Index is out of range.");
		return NULL;
	}

	return nodeList[nodeIndex];
}

/*
 * Add SkeletonNode object to [nodeList].
 */
void Skeleton::AddNodeToList(SkeletonNode * node)
{
	nodeList.push_back(node);
}

/*
 * Add a VertexBoneMap object to this skeleton.
 */
void Skeleton::AddVertexBoneMap(VertexBoneMap * map)
{
	vertexBoneMaps.push_back(map);
}

/*
 * Retrieve the VertexBoneMap object stored at [index] in [vertexBoneMaps].
 */
VertexBoneMap * Skeleton::GetVertexBoneMap(unsigned int index)
{
	if(index >= vertexBoneMaps.size())
	{
		Debug::PrintError("Skeleton::GetVertexBoneMap -> Index out of range.");
		return NULL;
	}

	return vertexBoneMaps[index];
}

/*
 * Create a full (deep) clone of this Skeleton object.
 */
Skeleton * Skeleton::FullClone()
{
	// allocate new Skeleton object
	Skeleton * newSkeleton = new Skeleton(boneCount);
	ASSERT(newSkeleton != NULL,"Skeleton::FullClone -> could not allocate skeleton",NULL);

	// initialize new skeleton
	bool initSuccess = newSkeleton->Init();
	if(!initSuccess)
	{
		delete newSkeleton;
		return NULL;
	}

	// copy over all the bones from this skeleton
	for(unsigned int i = 0; i < boneCount; i++)
	{
		newSkeleton->GetBone(i)->SetTo(GetBone(i));
	}

	// copy over the bone name map
	newSkeleton->boneNameMap = boneNameMap;

	// loop through each VertexBoneMap in this skeleton, create a full clone
	// of it, and then add to the new skeleton.
	for(unsigned int i = 0; i < vertexBoneMaps.size(); i++)
	{
		VertexBoneMap * existing = vertexBoneMaps[i];
		VertexBoneMap * clone = NULL;

		if(existing != NULL)
		{
			// create full clone of VertexBoneMap object
			clone = existing->FullClone();
			if(clone == NULL)
			{
				Debug::PrintError("Skeleton::FullClone -> Could not clone vertex bone map.");
				delete newSkeleton;
				return NULL;
			}
		}

		newSkeleton->vertexBoneMaps.push_back(clone);
	}

	// get the TreeNode object that contains the root node of the existing skeleton
	Tree<SkeletonNode *>::TreeNode * rootTreeNode = skeleton.GetRoot();

	if(rootTreeNode != NULL)
	{
		SkeletonNode * rootClone = NULL;

		// create a clone of the root SkeletonNode
		if(rootTreeNode->Data != NULL)
		{
			rootClone = rootTreeNode->Data->FullClone();
			if(rootClone == NULL)
			{
				Debug::PrintError("Skeleton::FullClone -> Could not clone root node.");
				delete newSkeleton;
				return NULL;
			}
		}

		// create new root TreeNode and set its Data to the new root SkeletonNode
		Tree<SkeletonNode *>::TreeNode * newRoot = newSkeleton->CreateRoot(rootClone);
		if(newRoot == NULL)
		{
			Debug::PrintError("Skeleton::FullClone -> Could not create root node.");
			delete newSkeleton;
			return NULL;
		}

		// map from TreeNode objects in the existing skeleton to their counterparts in the new skeleton
		std::unordered_map<Tree<SkeletonNode *>::TreeNode *, Tree<SkeletonNode *>::TreeNode *> newNodeMap;
		newNodeMap[rootTreeNode] = newRoot;

		bool allocateTreeSuccess = true;

		// Create a TreeNode visitor callback that will be used to visit each node in the existing skeleton in order to:
		// (1) create a copy of that TreeNode
		// (2) map the existing TreeNode to the corresponding TreeNode in the new skeleton.
		// After the existing skeleton tree is traversed using this visitor callback, the tree is not fully cloned. At that
		// point we will only have copies of the TreeNode objects in the skeleton tree, and not the SkeletonNode objects
		// that TreeNode::Data should point to. Additionally none of those TreeNode copies will have their children or parents
		// set yet, so there will be no hierarchy information available.
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

		// make sure Tree copied successfully
		if(!allocateTreeSuccess)
		{
			Debug::PrintError("Skeleton::FullClone -> Could not allocate new nodes for skeleton clone.");
			delete newSkeleton;
			return NULL;
		}

		// copy over the node name map
		newSkeleton->nodeNameMap = nodeNameMap;

		// pre-allocate nodeList space
		newSkeleton->nodeList.resize(nodeList.size());

		bool cloneTreeSuccess = true;
		Skeleton * thisSkeleton = this;

		// Create a second TreeNode visitor callback for the existing skeleton. The purpose of this callback is to:
		// (1) create a clone of the Data field of each TreeNode that is visited, which is the SkeletonNode object, and attach to
		//     the corresponding cloned node in the new skeleton.
		// (2) attach the cloned TreeNode in the new skeleton to its appropriate parent TreeNode in the new skeleton.
		skeleton.SetTraversalCallback([&cloneTreeSuccess, &newNodeMap, newSkeleton, thisSkeleton](Tree<SkeletonNode *>::TreeNode * node) -> bool
		{
			SkeletonNode * clonedSkeletonNode = NULL;
			if(node != NULL && node->Data != NULL)
			{
				// clone the SkeletonNode
				clonedSkeletonNode = node->Data->FullClone();
				if(clonedSkeletonNode == NULL)
				{
					Debug::PrintError("Skeleton::FullClone -> Could not clone node in skeletal tree.");
					cloneTreeSuccess = false;
					return NULL;
				}
			}

			// find the corresponding TreeNode in the new skeleton and attach the cloned SkeletonNode
			Tree<SkeletonNode *>::TreeNode * clonedTreeNode = newNodeMap[node];
			clonedTreeNode->Data = clonedSkeletonNode;

			// enter the cloned SkeletonNode in the [nodeList] of the new Skeleton object.
			int targetListIndex = thisSkeleton->GetNodeMapping(clonedSkeletonNode->Name);
			if(targetListIndex >= 0)newSkeleton->nodeList[targetListIndex] = clonedSkeletonNode;

			// the cloned TreeNode will still have a pointer to its original parent in the existing skeleton
			Tree<SkeletonNode *>::TreeNode * existingParent = clonedTreeNode->GetParent();

			// find counterpart of existing parent in the new skeleton, and add the cloned TreeNode as a child
			if(existingParent != NULL)
			{
				Tree<SkeletonNode *>::TreeNode * clonedParentNode = newNodeMap[existingParent];
				clonedParentNode->AddChild(clonedTreeNode);
			}

			// find the Bone object in the new skeleton that corresponds to the cloned SkeletonNode,
			// and set its SkeletonNode pointer [Node] to the clone SkeletonNode object.
			int boneIndex = node->Data->BoneIndex;
			if(boneIndex >= 0)
			{
				newSkeleton->GetBone(boneIndex)->Node = clonedSkeletonNode;
			}

			return true;
		});
		skeleton.Traverse();

		// check for successful clone
		if(!cloneTreeSuccess)
		{
			Debug::PrintError("Skeleton::FullClone -> Could not clone skeletal tree.");
			delete newSkeleton;
			return NULL;
		}
	}

	return newSkeleton;
}
