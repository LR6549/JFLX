#ifndef selectionSortHPP
#define selectionSortHPP

// Stats:   Best,   Average,    Worst,  Memory,     Stable
//          n²,     n²,         n²,     1,          No

#include <algorithm>

using namespace std;

namespace selectionSort {
    template <typename T>
    int findSmallest(const T& list, int min, int max) {
        int smallestIndex = min;
        for (int i = min + 1; i <= max; ++i) {
            if (list[i] < list[smallestIndex]) {
                smallestIndex = i;
            }
        }
        return smallestIndex;
    }

    /*
    * Structure/Container needs Indexing Abilities!
    */
    template <typename T>
    void selectionSort(T& list, bool inverted = false) {
        int length = list.size();
        if (inverted) {
            for (int i = length - 1; i > 0; --i) {
                int index = findSmallest(list, 0, i);
                swap(list[index], list[i]);
            }
        } else {
            for (int i = 0; i < length ; ++i) {
                int index = findSmallest(list, i, length - 1);
                swap(list[index], list[i]);
            }
        }
    }
}

#endif