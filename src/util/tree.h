#ifndef _TREE_H_
#define _TREE_H_

//forward declarations

#include <memory>
#include <vector>
#include <stack>
#include <string>

template <class T> class Tree
{
	public:

	class TreeNode
	{
		friend class Tree;
		std::vector<TreeNode *> children;

		public:

		T * Data;
	};

	private:

	TreeNode * root;

	bool fullTraversal;
	std::function<bool(T*)> traversalCallback;
	std::function<bool(T*)> fullTraversalCallback;

	public :

	Tree()
	{
		fullTraversal = false;
		root = NULL;
	}

    ~Tree()
    {

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
    	}
    }

    void Clear()
    {
    	fullTraversalCallback = [](TreeNode * node) -> bool
		{
    		delete node;
    		return true;
		};

    	FullTraverse();
    }

	private:

    void FullTraverse()
    {
    	if(root != NULL)
    	{
    		fullTraversal = true;
    	}
    }

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

    				childIndexStack.pop();
    				childIndexStack.push(childIndex+1);

    				iterationStack.push(next);
    				childIndexStack.push(0);
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
