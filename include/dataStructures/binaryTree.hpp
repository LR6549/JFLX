#ifndef BINARYTREE
#define BINARYTREE

#include <memory>

namespace binSearchTree
{
    template <typename T>
    class BinaryTree
    {
    public:
        T content;
        std::unique_ptr<BinaryTree<T>> leftTree;
        std::unique_ptr<BinaryTree<T>> rightTree;

        BinaryTree() = default;

        BinaryTree(const T& initialContent) : content(initialContent) {}

        bool isEmpty() const {
            return leftTree == nullptr && rightTree == nullptr;
        }

        const T &getContent() const
        {
            return content;
        }

        void setContent(const T& pContent)
        {
            content = pContent;
        }

        void setLeftTree(std::unique_ptr<BinaryTree<T>> pLeftTree)
        {
            leftTree = std::move(pLeftTree);
        }

        std::unique_ptr<BinaryTree<T>>& getLeftTree()
        {
            return leftTree;
        }

        void setRightTree(std::unique_ptr<BinaryTree<T>> pRightTree)
        {
            rightTree = std::move(pRightTree);
        }

        std::unique_ptr<BinaryTree<T>>& getRightTree()
        {
            return rightTree;
        }

        

    private:
    };
}

#endif