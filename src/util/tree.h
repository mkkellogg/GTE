#ifndef _TREE_H_
#define _TREE_H_

//forward declarations

#include <memory>
#include <vector>
#include <stack>
#include <string>
#include "global/global.h"
#include "ui/debug.h"

template <class T> class Tree
{
	public:

	class TreeNode
	{
		friend class Tree;
		std::vector<std::shared_ptr<TreeNode>> children;

		public:

		T * Data = NULL;

		std::weak_ptr<TreeNode> GetChild(unsigned int index)
		{
			if(index >= GetChildCount)return NULL;
			return children[index];
		}

		unsigned int GetChildCount()
		{
			return children.size();
		}

		std::weak_ptr<TreeNode> AddChild(T * data)
		{
			TreeNode * newNode = new TreeNode();
			NULL_CHECK(newNode,"Tree::AddChild -> Could not allocate new node", NULL);

			newNode->Data = data;
			std::shared_ptr<TreeNode> sharedRef(newNode);
			children.push_back(sharedRef);
			return sharedRef;
		}
	};

	private:

	std::shared_ptr<TreeNode> root;

	bool fullTraversal;
	std::function<bool(T*)> traversalCallback;
	std::function<bool(TreeNode*)> fullTraversalCallback;

	public :

	Tree()
	{
		fullTraversal = false;
	}

    ~Tree()
    {

    }

    std::weak_ptr<TreeNode> GetRoot()
	{
    	return root;
	}

    std::weak_ptr<TreeNode> AddRoot(T* data)
    {
    	if(root == NULL)
    	{
			TreeNode * newNode = new TreeNode();
			NULL_CHECK(newNode,"Tree::AddChild -> Could not allocate new node", NULL);

			newNode->Data = data;
			root = std::shared_ptr<TreeNode>(newNode);
    	}

		return root;
    }

    void SetTraversalCallback(std::function<bool(T*)> callback)
    {
    	this->traversalCallback = callback;
    }

    void Traverse()
    {
    	if(traversalCallback && root != NULL)
    	{
    		fullTraversal = false;
    		InternalTraverse();
    	}
    }

	private:

    void FullTraverse()
    {
    	if(root != NULL)
    	{
    		fullTraversal = true;
    		InternalTraverse();
    	}
    }

    // do post-order traversal
    void InternalTraverse()
    {
    	std::stack<std::shared_ptr<TreeNode>> iterationStack;
    	std::stack<unsigned int> childIndexStack;
    	if(root != NULL)
    	{
    		iterationStack.push(root);
    		childIndexStack.push(0);

    		while(iterationStack.size() > 0)
    		{
    			std::shared_ptr<TreeNode> current = iterationStack.top();
    			unsigned int childIndex = childIndexStack.top();

    			if(childIndex < current->children.size())
    			{
    				std::shared_ptr<TreeNode> next = current->children[childIndex];

    				if(next)
    				{
						childIndexStack.pop();
						childIndexStack.push(childIndex+1);

						iterationStack.push(next);
						childIndexStack.push(0);
    				}
    				else
    				{
    					Debug::PrintWarning("Tree::InternalTraverse -> NULL node encountered.");
    				}
    			}
    			else
    			{
    				bool continueTraversal = true;
    				if(fullTraversal)
    				{
    					if(fullTraversalCallback)
    					{
    						continueTraversal = fullTraversalCallback(&*current);
    					}
    				}
    				else
    				{
    					if(traversalCallback)
    					{
    						continueTraversal = traversalCallback(current->Data);
    					}
    				}
    				if(!continueTraversal)break;

    				iterationStack.pop();
    				childIndexStack.pop();
    			}
    		}
    	}
    }

};

#endif
