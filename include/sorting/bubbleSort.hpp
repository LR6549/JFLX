#ifndef bubbleSortHPP
#define bubbleSortHPP

// Stats:   Best,   Average,    Worst,  Memory,     Stable
//          n²,     n²,         n²,     1,          No

#include <algorithm>

using namespace std;

namespace bubbleSort {
    /*
    * Structure/Container needs Indexing Abilities!
    */
    template <typename T>
    void bubbleSort(T& list, bool inverted = false, bool optimized = false) {
        int length = list.size();
        bool swapped;

        if (optimized) {
            if (inverted) {
                do {
                    swapped = false;
                    for (int i = 0; i < length - 1; ++i) {
                        if (list[i] < list[i + 1]) {
                            swap(list[i], list[i + 1]);
                            swapped = true;
                        }
                    }
                    --length;
                } while (swapped);
            } else {
                do {
                    swapped = false;
                    for (int i = 0; i < length - 1; ++i) {
                        if (list[i] > list[i + 1]) {
                            swap(list[i], list[i + 1]);
                            swapped = true;
                        }
                    }
                    --length;
                } while (swapped);
            }
        } else {
            if (inverted) {
                do {
                    swapped = false;
                    for (int i = 0; i < length - 1; ++i) {
                        if (list[i] < list[i + 1]) {
                            swap(list[i], list[i + 1]);
                            swapped = true;
                        }
                    }
                } while (swapped);
            } else {
                do {
                    swapped = false;
                    for (int i = 0; i < length - 1; ++i) {
                        if (list[i] > list[i + 1]) {
                            swap(list[i], list[i + 1]);
                            swapped = true;
                        }
                    }
                } while (swapped);
            }
        }
    }
}

#endif