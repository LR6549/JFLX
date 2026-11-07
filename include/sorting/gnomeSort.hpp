#ifndef gnomeSortHPP
#define gnomeSortHPP

// Stats:   Best,   Average,    Worst,  Memory,     Stable
//          n,      n²,         n²,     1,          Yes

#include <algorithm>

using namespace std;

namespace gnomeSort {
    /*
    * Structure/Container needs Indexing Abilities!
    */
    template <typename T>
    void gnomeSort(T& list, bool inverted = false) {
        int length = list.size();
        if (inverted) {
            int i = length;
            while(i > 0) {
                if (i == length || list[i] <= list[i-1]) {
                    i --;
                } else {
                    swap(list[i], list[i+1]);
                    i ++;
                }
            }
        } else {
            int i = 0;
            while(i < length) {
                if (i == 0 || list[i] >= list[i-1]) {
                    i ++;
                } else {
                    swap(list[i], list[i-1]);
                    i --;
                }
            }
        }
    }
}

#endif