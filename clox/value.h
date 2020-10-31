#ifndef VALUE_H
#define VALUE_H

#include "common.h"
#include <cstdio>

typedef enum {
  VAL_BOOL,
  VAL_NIL,
  VAL_NUMBER
} ValueType;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
  } as;
} Value;


// comparing macros if type for type-matches
#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)

// use a value as bool
#define AS_BOOL(value)    ((value).as.boolean)
// ... and number
#define AS_NUMBER(value)  ((value).as.number)

// native bool to lox
#define BOOL_VAL(value)   ((Value){VAL_BOOL, {.boolean = value}})
// native NIL == 0
#define NIL_VAL           ((Value){VAL_NIL, {.number = 0}})
// native double == number
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);

void printValue(Value value);

class ValueObj {
private:
    typedef struct {
        using ValueType = double;
        size_t capacity;
        size_t count;
        ValueType* values;
    } ValArray;

    ValArray* arr;

public:
    ValueObj(ValArray* Arr)
        : arr(Arr)
    { // configure ValArray in ctor
        // INIT STUFF
    }
    void write(ValArray* array, Value Val);

    ~ValueObj()
    {
        // DELETE STUFF
    }
};

#endif // VALUE_H
