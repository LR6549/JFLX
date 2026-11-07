#ifndef GenericStackHPP
#define GenericStackHPP

#include <stdexcept>

using namespace std;

/*
* Principle:
* - First In Last Out -> LI LO
*/
namespace genericStack {
    template <typename T>
    struct StackNode {
        T content;
        StackNode<T>* previousNode;

        StackNode(const T& tempContent, StackNode<T>* tempStackNode = nullptr) 
            : content(tempContent), previousNode(tempStackNode) {}

        void setPreviousStackNode(StackNode<T>* tempStackNode) {
            previousNode = tempStackNode;
        }

        void setContent(const T& tempContent) {
            content = tempContent;
        }

        T getContent() const {
            return content;
        }

        StackNode<T>* getPreviousStackNode() const {
            return previousNode;
        }
    };

    template <typename T>
    struct Stack {
        StackNode<T>* currentStackNode;

        Stack() : currentStackNode(nullptr) {}

        bool isEmpty() const {
            return currentStackNode == nullptr;
        }

        void push(const T& tempContent) {
            currentStackNode = new StackNode<T>(tempContent, currentStackNode);
        }

        void pop() {
            if (!isEmpty()) {
                StackNode<T>* oldStackNode = currentStackNode;
                currentStackNode = currentStackNode->getPreviousStackNode();
                delete oldStackNode;
            }
        }

        T top() const {
            if (isEmpty()) {
                throw out_of_range("Stack is empty.");
            }
            return currentStackNode->getContent();
        }

        ~Stack() {
            while (!isEmpty()) {
                pop();
            }
        }
    };
}

#endif
