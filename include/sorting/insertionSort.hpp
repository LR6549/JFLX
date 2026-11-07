#ifndef insertionSortHPP
#define insertionSortHPP

// Stats:   Best,   Average,    Worst,  Memory,     Stable
//          n,      n²,         n²,     1,          No

#include <algorithm>

using namespace std;

namespace insertionSort {
    /*
    * Structure/Container needs Indexing Abilities!
    */
    template <typename T>
    void insertionSort(T& list, bool inverted = false, bool optimized = false) {
        int length = list.size();
        if (inverted) {
            for (int i = 1; i < length; ++i) {
                int j = i;
                while (j > 0 && list[j] > list[j - 1]) {
                    swap(list[j], list[j - 1]);
                    --j;
                }
            }
        } else {
            for (int i = 1; i < length; ++i) {
                int j = i;
                while (j > 0 && list[j] < list[j - 1]) {
                    swap(list[j], list[j - 1]);
                    --j;
                }
            }
        }
    }
}

#endif