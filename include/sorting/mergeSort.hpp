#ifndef mergeSortHPP
#define mergeSortHPP

// Stats:   Best,   Average,    Worst,  Memory,     Stable
//      n log(n),   n log(n), n log(n), n,          Yes

#include <algorithm>
#include <iterator>
#include <type_traits>

using namespace std;

namespace mergeSort {
    template <typename T>
    T topDownMerge(T leftList, T rightList) {
        T result;

        auto leftIt = leftList.begin();
        auto rightIt = rightList.begin();

        while (leftIt != leftList.end() && rightIt != rightList.end()) {
            if (*leftIt <= *rightIt) {
                result.insert(result.end(), *leftIt);
                ++leftIt;
            } else {
                result.insert(result.end(), *rightIt);
                ++rightIt;
            }
        }

        result.insert(result.end(), leftIt, leftList.end());
        result.insert(result.end(), rightIt, rightList.end());

        return result;
    }

    template <typename T>
    T topDownMergeSort(T& list) {
        T leftList;
        T rightList;

        if (list.size() <= 1) {
            return list;
        }

        for (auto it = list.begin(); it < list.end(); ++it) {
            if (distance(list.begin(), it) < list.size() / 2) {
                leftList.insert(leftList.end(), *it);
            } else {
                rightList.insert(rightList.end(), *it);
            }
        }

        leftList = topDownMergeSort(leftList);
        rightList = topDownMergeSort(rightList);

        return topDownMerge(leftList, rightList);
    }

    template <typename T>
    T topDownInvertedMerge(T leftList, T rightList) {
        T result;

        auto leftIt = leftList.begin();
        auto rightIt = rightList.begin();

        while (leftIt != leftList.end() && rightIt != rightList.end()) {
            if (*leftIt >= *rightIt) {
                result.insert(result.end(), *leftIt);
                ++leftIt;
            } else {
                result.insert(result.end(), *rightIt);
                ++rightIt;
            }
        }

        result.insert(result.end(), leftIt, leftList.end());
        result.insert(result.end(), rightIt, rightList.end());

        return result;
    }

    template <typename T>
    T topDownInvertedMergeSort(T& list) {
        T leftList;
        T rightList;

        if (list.size() <= 1) {
            return list;
        }

        for (auto it = list.begin(); it < list.end(); ++it) {
            if (distance(list.begin(), it) < list.size() / 2) {
                leftList.insert(leftList.end(), *it);
            } else {
                rightList.insert(rightList.end(), *it);
            }
        }

        leftList = topDownInvertedMergeSort(leftList);
        rightList = topDownInvertedMergeSort(rightList);

        return topDownInvertedMerge(leftList, rightList);
    }

    /*
    * Structure/Container needs Indexing Abilities!
    */
    template <typename T>
    void mergeSort(T& list, bool inverted = false) {
        if (inverted) {
            list = topDownInvertedMergeSort(list);
        } else {
            list = topDownMergeSort(list);
        }
    }
}

#endif