#ifndef VM_H
#define VM_H
#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* ip; // instruction pointer, we could also call it PC for program counter
    Value stack[STACK_MAX];
    Value* stackTop;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM();

InterpretResult interpret(const char* source);

void push(Value value);
Value pop();

void freeVM();
#endif // VM_H
