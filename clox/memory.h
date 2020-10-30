#ifndef clox_memory_h
#define clox_memory_h
#include <cstddef>

// set capacity of struct
#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity)*2)

// Grow an array by reallocating it's size
#define GROW_ARRAY(previous, type, oldCount, count)                    \
    static_cast<type*>(reallocate(previous, sizeof(type) * (oldCount), \
        sizeof(type) * (count)))

// Free an array
#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

/**
 * @brief reallocate either frees an array when newSize==0, or reallocates it's size with realloc()
 * @param previous
 * @param oldSize
 * @param newSize
 * @return
 */
void* reallocate(void* previous, size_t oldSize, size_t newSize);
#endif
