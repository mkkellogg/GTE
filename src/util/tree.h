#ifndef _GTE_TREE_H_
#define _GTE_TREE_H_

#include "engine.h"
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"

#include <vector>
#include <stack>
#include <string>

namespace GTE {
    template <class T> class Tree {
    public:

        class TreeNode {
            friend class Tree;
            std::vector<TreeNode *> children;
            TreeNode * parent = nullptr;

        public:

            T Data;

            TreeNode() {

            }

            TreeNode(T data) : Data(data) {

            }

            TreeNode* GetChild(UInt32 index) {
                if (index >= GetChildCount())return nullptr;
                return children[index];
            }

            UInt32 GetChildCount() {
                return children.size();
            }

            TreeNode* AddChild(T data) {
                TreeNode * newNode = new TreeNode(data);
                ASSERT(newNode != nullptr, "Tree::AddChild -> Could not allocate new node");
                newNode->parent = this;
                children.push_back(newNode);
                return newNode;
            }

            TreeNode* AddChild(TreeNode * node) {
                node->parent = this;
                children.push_back(node);
                return node;
            }

            TreeNode* GetParent() {
                return parent;
            }
        };

    private:

        TreeNode * root;

        Bool fullTraversal;
        std::function<Bool(TreeNode*)> traversalCallback;
        std::function<Bool(TreeNode*)> fullTraversalCallback;


    public:

        Tree() {
            root = nullptr;
            fullTraversal = false;
        }

        ~Tree() {

        }

        TreeNode* GetRoot() {
            return root;
        }

        TreeNode* AddRoot(T data) {
            if (!root) {
                TreeNode * newNode = new TreeNode(data);
                ASSERT(newNode != nullptr, "Tree::AddChild -> Could not allocate new node");

                root = newNode;
            }

            return root;
        }

        void SetTraversalCallback(std::function<Bool(TreeNode*)> callback) {
            this->traversalCallback = callback;
        }

        void Traverse() {
            if (traversalCallback && root != nullptr) {
                fullTraversal = false;
                InternalTraverse();
            }
        }

    private:

        void FullTraverse() {
            if (root != nullptr && fullTraversalCallback) {
                fullTraversal = true;
                InternalTraverse();
            }
        }

        // do post-order traversal
        void InternalTraverse() {
            std::stack<TreeNode *> iterationStack;
            std::stack<UInt32> childIndexStack;
            if (root != nullptr) {
                iterationStack.push(root);
                childIndexStack.push(0);

                while (iterationStack.size() > 0) {
                    TreeNode * current = iterationStack.top();
                    UInt32 childIndex = childIndexStack.top();

                    if (childIndex < current->children.size()) {
                        TreeNode * next = current->children[childIndex];

                        if (next) {
                            childIndexStack.pop();
                            childIndexStack.push(childIndex + 1);

                            iterationStack.push(next);
                            childIndexStack.push(0);
                        }
                        else {
                            Debug::PrintWarning("Tree::InternalTraverse -> Null node encountered.");
                        }
                    }
                    else {
                        Bool continueTraversal = true;
                        if (fullTraversal) {
                            if (fullTraversalCallback) {
                                continueTraversal = fullTraversalCallback(current);
                            }
                        }
                        else {
                            if (traversalCallback) {
                                continueTraversal = traversalCallback(current);
                            }
                        }
                        if (!continueTraversal)break;

                        iterationStack.pop();
                        childIndexStack.pop();
                    }
                }
            }
        }
    };
}

#endif
