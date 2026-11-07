#ifndef bogoSortHPP
#define bogoSortHPP

// Stats:   Best,   Average,    Worst,  Memory,     Stable
//          n,     nxn!,         n²,     1,          No

#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

namespace bogoSort {
    template <typename T>
    bool isSorted(const T& list, int length) {
        for (int i = 1; i < length; ++i) {
            if (list[i - 1] > list[i]) {
                return false;
            }
        }
        return true;
    }

    /*
    * Structure/Class needs Indexing Abilities!
    */
    template <typename T>
    void bogoSort(T& list) {
        srand(static_cast<unsigned int>(time(0)));
        int length = list.size();

        while (!isSorted(list, length)) {
            for (int i = 0; i < length; ++i) {
                swap(list[i], list[rand() % length]);
            }
        }
    }
}

#endif