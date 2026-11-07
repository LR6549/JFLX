#ifndef GenericQueueHPP
#define GenericQueueHPP

#include <stdexcept>

using namespace std;

/*
* Principle:
* - First In First Out -> Fi FO
*/
namespace genericQueue {
    template <typename T>
    struct QueueNode {
        T content;
        QueueNode<T>* nextNode;

        QueueNode(const T& tempContent, QueueNode<T>* tempQueueNode = nullptr) 
            : content(tempContent), nextNode(tempQueueNode) {}

        void setNextQueueNode(QueueNode<T>* tempQueueNode) {
            nextNode = tempQueueNode;
        }

        void setContent(const T& tempContent) {
            content = tempContent;
        }

        T getContent() const {
            return content;
        }

        QueueNode<T>* getNextQueueNode() const {
            return nextNode;
        }
    };

    template <typename T>
    struct Queue {
        QueueNode<T>* headQueueNode;
        QueueNode<T>* tailQueueNode;

        Queue() : headQueueNode(nullptr), tailQueueNode(nullptr) {}

        bool isEmpty() const {
            return headQueueNode == nullptr;
        }

        void enqueue(const T& tempContent) {
            QueueNode<T>* newQueueNode = new QueueNode<T>(tempContent);
            if (isEmpty()) {
                headQueueNode = newQueueNode;
            } else {
                tailQueueNode->setNextQueueNode(newQueueNode);
            }
            tailQueueNode = newQueueNode;
        }

        void dequeue() {
            if (!isEmpty()) {
                QueueNode<T>* oldQueueNode = headQueueNode;
                headQueueNode = headQueueNode->getNextQueueNode();
                delete oldQueueNode;
                if (headQueueNode == nullptr) {
                    tailQueueNode = nullptr;
                }
            } else {
                throw out_of_range("Queue is empty.");
            }
        }

        T first() const {
            if (isEmpty()) {
                throw out_of_range("Queue is empty.");
            }
            return headQueueNode->getContent();
        }

        ~Queue() {
            while (!isEmpty()) {
                dequeue();
            }
        }
    };
}

#endif
