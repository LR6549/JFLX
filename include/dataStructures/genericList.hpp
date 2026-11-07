#ifndef GenericListHPP
#define GenericListHPP

#include <stdexcept>

using namespace std;

/*
* Principle:
* - Any In Any Out -> AI AO
*/
namespace genericList {
    template <typename T>
    struct ListNode {
        T content;
        ListNode<T>* nextNode;

        ListNode(const T& tempContent, ListNode<T>* tempNextNode = nullptr)
            : content(tempContent), nextNode(tempNextNode) {}

        void setNextNode(ListNode<T>* tempNextNode) {
            nextNode = tempNextNode;
        }

        void setContent(const T& tempContent) {
            content = tempContent;
        }

        T getContent() const {
            return content;
        }

        ListNode<T>* getNextNode() const {
            return nextNode;
        }
    };

    template <typename T>
    struct List {
        ListNode<T>* first;
        ListNode<T>* last;
        ListNode<T>* current;

        List() : first(nullptr), last(nullptr), current(nullptr) {}

        bool isEmpty() const {
            return first == nullptr;
        }

        bool hasAccess() const {
            return current != nullptr;
        }

        void next() {
            if (hasAccess()) {
                current = current->getNextNode();
            }
        }

        void toFirst() {
            if (!isEmpty()) {
                current = first;
            }
        }

        void toLast() {
            if (!isEmpty()) {
                current = last;
            }
        }

        T getContent() const {
            if (hasAccess()) {
                return current->getContent();
            }
            throw out_of_range("No current element.");
        }

        void setContent(const T& tempContent) {
            if (hasAccess()) {
                current->setContent(tempContent);
            }
        }

        void insert(const T& tempContent) {
            if (tempContent != T()) {
                if (hasAccess()) {
                    ListNode<T>* newNode = new ListNode<T>(tempContent);
                    if (current != first) {
                        ListNode<T>* previous = getPrevious(current);
                        newNode->setNextNode(previous->getNextNode());
                        previous->setNextNode(newNode);
                    } else {
                        newNode->setNextNode(first);
                        first = newNode;
                    }
                } else {
                    if (isEmpty()) {
                        ListNode<T>* newNode = new ListNode<T>(tempContent);
                        first = newNode;
                        last = newNode;
                    }
                }
            }
        }

        void append(const T& tempContent) {
            if (tempContent != T()) {
                if (isEmpty()) {
                    insert(tempContent);
                } else {
                    ListNode<T>* newNode = new ListNode<T>(tempContent);
                    last->setNextNode(newNode);
                    last = newNode;
                }
            }
        }

        void concat(List<T>& otherList) {
            if (this != &otherList && !otherList.isEmpty()) {
                if (isEmpty()) {
                    first = otherList.first;
                    last = otherList.last;
                } else {
                    last->setNextNode(otherList.first);
                    last = otherList.last;
                }
                otherList.first = nullptr;
                otherList.last = nullptr;
                otherList.current = nullptr;
            }
        }

        void remove() {
            if (hasAccess() && !isEmpty()) {
                if (current == first) {
                    first = first->getNextNode();
                } else {
                    ListNode<T>* previous = getPrevious(current);
                    if (current == last) {
                        last = previous;
                    }
                    previous->setNextNode(current->getNextNode());
                }

                ListNode<T>* temp = current->getNextNode();
                delete current;
                current = temp;

                if (isEmpty()) {
                    last = nullptr;
                }
            }
        }

        ListNode<T>* getPrevious(ListNode<T>* pListNode) {
            if (pListNode != nullptr && pListNode != first && !isEmpty()) {
                ListNode<T>* temp = first;
                while (temp != nullptr && temp->getNextNode() != pListNode) {
                    temp = temp->getNextNode();
                }
                return temp;
            }
            return nullptr;
        }

        ~List() {
            while (!isEmpty()) {
                remove();
            }
        }
    };
}

#endif