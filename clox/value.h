#ifndef VALUE_H
#define VALUE_H

#include "common.h"
#include <cstdio>

typedef double Value;

typedef struct {
  int capacity;
  int count;
  Value *values;
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
  
  ValArray *arr;
  
public:
  ValueObj(ValArray *Arr) : arr(Arr) { // configure ValArray in ctor
    // INIT STUFF
  }
  void write(ValArray *array, Value Val);
  
  ~ValueObj() {
    // DELETE STUFF
  }
    

};

#endif // VALUE_H
