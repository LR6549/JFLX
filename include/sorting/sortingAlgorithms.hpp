#ifndef JFLX_SORTINGALGORITHMS
#define JFLX_SORTINGALGORITHMS

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iterator>
#include <type_traits>
#include <utility>     // std::swap (qualified calls inside the algorithms)

/*
* All sorting objects (list, vector, ...) need Idexing Abilities, as well as a default constructor!
*
* Every algorithm is exposed as JFLX::sort::<algorithm>Sort(list, inverted = false):
*   mergeSort, quickSort, selectionSort, insertionSort, gnomeSort,
*   bubbleSort, heapSort, bogoSort
* Helpers live in JFLX::sort::detail.
*/
namespace JFLX::sort {

    namespace detail {

        /*
        *   Stats:
        *   | Best         | Average      | Worst        | Memory     | Stable     |
        *   |--------------|--------------|--------------|------------|------------|
        *   | O(n log(n))  | O(n log(n))  | O(n log(n))  | n          | Yes        |
        */
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
                if (static_cast<decltype(list.size())>(distance(list.begin(), it)) < list.size() / 2) {
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
                if (static_cast<decltype(list.size())>(distance(list.begin(), it)) < list.size() / 2) {
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
        *   Stats:
        *   | Best        | Average     | Worst       | Memory     | Stable     |
        *   |-------------|-------------|-------------|------------|------------|
        *   | O(n log(n)) | O(n log(n)) | O(n²)       | log(n)     | No         |
        */
        template <typename T>
        int partition(T& list, int low, int high) {
            auto pivot = list[low];
            int i = low - 1;
            int j = high + 1;

            while (true) {
                do {
                    i++;
                } while (list[i] < pivot);

                do {
                    j--;
                } while (list[j] > pivot);

                if (i >= j) {
                    return j;
                }

                std::swap(list[i], list[j]);
            }
        }

        template <typename T>
        int partitionInverted(T& list, int low, int high) {
            auto pivot = list[low];
            int i = low - 1;
            int j = high + 1;

            while (true) {
                do {
                    i++;
                } while (list[i] > pivot);

                do {
                    j--;
                } while (list[j] < pivot);

                if (i >= j) {
                    return j;
                }

                std::swap(list[i], list[j]);
            }
        }

        template <typename T>
        void quickSort(T& list, int low, int high) {
            if (low >= 0 && high >= 0 && low < high) {
                int p = partition(list, low, high);
                quickSort(list, low, p);
                quickSort(list, p + 1, high);
            }
        }

        template <typename T>
        void quickSortInverted(T& list, int low, int high) {
            if (low >= 0 && high >= 0 && low < high) {
                int p = partitionInverted(list, low, high);
                quickSortInverted(list, low, p);
                quickSortInverted(list, p + 1, high);
            }
        }

        /*
        *   Stats:
        *   | O(n²)       | O(n²)       | O(n²)       |   1        | No         |
        */
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
        *   Stats:
        *   | Best,   Average,    Worst,  Memory,     Stable
        *   | n log(n), n log(n), n log(n),  1,     No
        */
        template <typename T>
        void heapify(T& list, int n, int i) {
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
                std::swap(list[i], list[largest]);

                heapify(list, n, largest);
            }
        }

        template <typename T>
        void heapSort(T& list, int length) {
            for (int i = length / 2 - 1; i >= 0; i--) {
                heapify(list, length, i);
            }

            for (int i = length - 1; i > 0; i--) {
                std::swap(list[0], list[i]);

                heapify(list, i, 0);
            }
        }

        template <typename T>
        void heapifyInverted(T& list, int n, int i) {
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
                std::swap(list[i], list[smallest]);

                heapifyInverted(list, n, smallest);
            }
        }

        template <typename T>
        void heapSortInverted(T& list, int length) {
            for (int i = length / 2 - 1; i >= 0; i--) {
                heapifyInverted(list, length, i);
            }

            for (int i = length - 1; i > 0; i--) {
                std::swap(list[0], list[i]);

                heapifyInverted(list, i, 0);
            }
        }

        /*
        *   Stats:
        *   | Best,   Average,    Worst,  Memory,     Stable
        *   | n,     nxn!,         n²,     1,          No
        */
        template <typename T>
        bool isSorted(const T& list, int length) {
            for (int i = 1; i < length; ++i) {
                if (list[i - 1] > list[i]) {
                    return false;
                }
            }
            return true;
        }

    } // namespace detail

    /*
    *   Stats:
    *   | Best         | Average      | Worst        | Memory     | Stable     |
    *   |--------------|--------------|--------------|------------|------------|
    *   | O(n log(n))  | O(n log(n))  | O(n log(n))  | n          | Yes        |
    */
    template <typename T>
    void mergeSort(T& list, bool inverted = false) {
        if (inverted) {
            list = detail::topDownInvertedMergeSort(list);
        } else {
            list = detail::topDownMergeSort(list);
        }
    }

    /*
    *   Stats:
    *   | Best        | Average     | Worst       | Memory     | Stable     |
    *   |-------------|-------------|-------------|------------|------------|
    *   | O(n log(n)) | O(n log(n)) | O(n²)       | log(n)     | No         |
    */
    template <typename T>
    void quickSort(T& list, bool inverted = false) {
        int length = list.size();
        if (inverted) {
            detail::quickSortInverted(list, 0, length - 1);
        } else {
            detail::quickSort(list, 0, length - 1);
        }
    }

    /*
    *   Stats:
    *   | O(n²)       | O(n²)       | O(n²)       |   1        | No         |
    */
    template <typename T>
    void selectionSort(T& list, bool inverted = false) {
        int length = list.size();
        if (inverted) {
            for (int i = length - 1; i > 0; --i) {
                int index = detail::findSmallest(list, 0, i);
                std::swap(list[index], list[i]);
            }
        } else {
            for (int i = 0; i < length ; ++i) {
                int index = detail::findSmallest(list, i, length - 1);
                std::swap(list[index], list[i]);
            }
        }
    }

    /*
    *   Stats:
    *   | Best        | Average     | Worst       | Memory     | Stable     |
    *   |-------------|-------------|-------------|------------|------------|
    *   | n           | n²          | n²          | 1          | No         |
    */
    template <typename T>
    void insertionSort(T& list, bool inverted = false, bool optimized = false) {
        int length = list.size();
        if (inverted) {
            for (int i = 1; i < length; ++i) {
                int j = i;
                while (j > 0 && list[j] > list[j - 1]) {
                    std::swap(list[j], list[j - 1]);
                    --j;
                }
            }
        } else {
            for (int i = 1; i < length; ++i) {
                int j = i;
                while (j > 0 && list[j] < list[j - 1]) {
                    std::swap(list[j], list[j - 1]);
                    --j;
                }
            }
        }
    }

    /*
    *   Stats:
    *   | Best        | Average     | Worst       | Memory     | Stable     |
    *   |-------------|-------------|-------------|------------|------------|
    *   | n           | n²          | n²          | 1          | Yes        |
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
                    std::swap(list[i], list[i+1]);
                    i ++;
                }
            }
        } else {
            int i = 0;
            while(i < length) {
                if (i == 0 || list[i] >= list[i-1]) {
                    i ++;
                } else {
                    std::swap(list[i], list[i-1]);
                    i --;
                }
            }
        }
    }

    /*
    *   Stats:
    *   | Best        | Average     | Worst       | Memory     | Stable     |
    *   |-------------|-------------|-------------|------------|------------|
    *   | n²          | n²          | n²          | 1          | No         |
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
                            std::swap(list[i], list[i + 1]);
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
                            std::swap(list[i], list[i + 1]);
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
                            std::swap(list[i], list[i + 1]);
                            swapped = true;
                        }
                    }
                } while (swapped);
            } else {
                do {
                    swapped = false;
                    for (int i = 0; i < length - 1; ++i) {
                        if (list[i] > list[i + 1]) {
                            std::swap(list[i], list[i + 1]);
                            swapped = true;
                        }
                    }
                } while (swapped);
            }
        }
    }

    /*
    * Structure/Container needs Indexing Abilities!
    */
    template <typename T>
    void heapSort(T& list, bool inverted = false) {
        int length = list.size();
        if (inverted) {
            detail::heapSortInverted(list, length);
        } else {
            detail::heapSort(list, length);
        }
    }

    /*
    * Structure/Class needs Indexing Abilities!
    */
    template <typename T>
    void bogoSort(T& list) {
        srand(static_cast<unsigned int>(time(0)));
        int length = list.size();

        while (!detail::isSorted(list, length)) {
            for (int i = 0; i < length; ++i) {
                std::swap(list[i], list[rand() % length]);
            }
        }
    }
}

#endif
