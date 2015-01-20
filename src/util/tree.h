#ifndef _GTE_TREE_H_
#define _GTE_TREE_H_

//forward declarations

#include <memory>
#include <vector>
#include <stack>
#include <string>
#include "global/global.h"
#include "debug/gtedebug.h"

template <class T> class Tree
{
	public:

	class TreeNode
	{
		friend class Tree;
		std::vector<TreeNode *> children;
		TreeNode * parent = NULL;

		public:

		T Data;

		TreeNode()
		{

		}

		TreeNode(T data) : Data(data)
		{

		}

		TreeNode* GetChild(unsigned int index)
		{
			if(index >= GetChildCount())return NULL;
			return children[index];
		}

		unsigned int GetChildCount()
		{
			return children.size();
		}

		TreeNode* AddChild(T data)
		{
			TreeNode * newNode = new TreeNode(data);
			ASSERT(newNode != NULL,"Tree::AddChild -> Could not allocate new node", NULL);
			newNode->parent = this;
			children.push_back(newNode);
			return newNode;
		}

		TreeNode* AddChild(TreeNode * node)
		{
			node->parent = this;
			children.push_back(node);
			return node;
		}

		TreeNode* GetParent()
		{
			return parent;
		}
	};

	private:

	TreeNode* root;

	bool fullTraversal;
	std::function<bool(TreeNode*)> traversalCallback;
	std::function<bool(TreeNode*)> fullTraversalCallback;


	public :

	Tree()
	{
		root = NULL;
		fullTraversal = false;
	}

    ~Tree()
    {

    }

    TreeNode* GetRoot()
	{
    	return root;
	}

    TreeNode* AddRoot(T data)
    {
    	if(!root)
    	{
			TreeNode * newNode = new TreeNode(data);
			ASSERT(newNode != NULL,"Tree::AddChild -> Could not allocate new node", NULL);

			root = newNode;
    	}

		return root;
    }

    void SetTraversalCallback(std::function<bool(TreeNode*)> callback)
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
    	if(root != NULL && fullTraversalCallback)
    	{
    		fullTraversal = true;
    		InternalTraverse();
    	}
    }

    // do post-order traversal
    void InternalTraverse()
    {
    	std::stack<TreeNode *> iterationStack;
    	std::stack<unsigned int> childIndexStack;
    	if(root != NULL)
    	{
    		iterationStack.push(root);
    		childIndexStack.push(0);

    		while(iterationStack.size() > 0)
    		{
    			TreeNode * current = iterationStack.top();
    			unsigned int childIndex = childIndexStack.top();

    			if(childIndex < current->children.size())
    			{
    				TreeNode * next = current->children[childIndex];

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
    						continueTraversal = fullTraversalCallback(current);
    					}
    				}
    				else
    				{
    					if(traversalCallback)
    					{
    						continueTraversal = traversalCallback(current);
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
