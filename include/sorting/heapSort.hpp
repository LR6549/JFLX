#ifndef heapSortHPP
#define heapSortHPP

// Stats:   Best,   Average,    Worst,  Memory,     Stable
//      n log(n),   n log(n), n log(n),  1,     No

#include <algorithm>
#include <vector>

using namespace std;

namespace heapSort {
    template <typename T>
    void heapify(vector<T>& list, int n, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && list[left] > list[largest]) {
            largest = left;
        }

        if (right < n && list[right] > list[largest]) {
            largest = right;
        }

        if (largest != i) {
            swap(list[i], list[largest]);

            heapify(list, n, largest);
        }
    }

    template <typename T>
    void heapSort(vector<T>& list, int length) {
        for (int i = length / 2 - 1; i >= 0; i--) {
            heapify(list, length, i);
        }

        for (int i = length - 1; i > 0; i--) {
            swap(list[0], list[i]);

            heapify(list, i, 0);
        }
    }
    
    template <typename T>
    void heapifyInverted(vector<T>& list, int n, int i) {
        int smallest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && list[left] < list[smallest]) {
            smallest = left;
        }

        if (right < n && list[right] < list[smallest]) {
            smallest = right;
        }

        if (smallest != i) {
            swap(list[i], list[smallest]);

            heapifyInverted(list, n, smallest);
        }
    }

    template <typename T>
    void heapSortInverted(vector<T>& list, int length) {
        for (int i = length / 2 - 1; i >= 0; i--) {
            heapifyInverted(list, length, i);
        }

        for (int i = length - 1; i > 0; i--) {
            swap(list[0], list[i]);

            heapifyInverted(list, i, 0);
        }
    }

    /*
    * Structure/Container needs Indexing Abilities!
    */
    template <typename T>
    void heapSort(T& list, bool inverted = false) {
        int length = list.size();
        if (inverted) {
            heapSortInverted(list, length);
        } else {
            heapSort(list, length);
        }
    }
}

#endif