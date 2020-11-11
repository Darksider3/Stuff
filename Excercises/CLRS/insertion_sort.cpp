#include <iostream>

template<typename SortT, typename LenT = size_t>
SortT* insertion_sort(SortT* list, LenT size)
{
    if (size < 2) {
        return list;
    }

    for (LenT j = 2; j < size; ++j) {
        SortT keyobj = list[j];

        LenT i = j - 1;
        while (i > 0 && list[i] > keyobj) {
            list[i + 1] = list[i];
            i = i - 1;
        }
        list[i + 1] = keyobj;
    }

    return list;
}

int* insertion_sort(int* list, size_t len)
{
    if (len < 2)
        return list;

    for (size_t j = 2; j < len; ++j) {
        int key = list[j];
        // insert list[j] into the sorted sequence A[1...j-1].
        size_t i = j - 1;
        while (i > 0 && list[i] > key) {
            list[i + 1] = list[i];
            i = i - 1;
        }
        list[i + 1] = key;
    }

    return list;
}

int main(int argc, char** argv)
{
    int ab[] = { 0, 3, 1, 2 };
    size_t size = 4;

    insertion_sort<int, size_t>(ab, size);

    //insertion_sort(ab, size);
    for (size_t i = 0; i < size; ++i) {
        std::cout << ab[i] << "\n";
    }

    std::cout << std::endl;
    return 0;
}
