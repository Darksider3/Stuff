#include "vm.h"
#include "common.h"
#include "debug.h"
#include <cstdio>

// for convenience, define it here.
// we'll have to pass it around the whole file probably
// and it would be very abyssimal to do so manually
// in every freakin' function

// @TODO: later, pass it around!
VM vm;

static void resetStack()
{
  vm.stackTop = vm.stack;
}

void push(Value value)
{
  *vm.stackTop = value; // store value on stackTop..
  vm.stackTop++; // and advance it
}

Value pop()
{
  vm.stackTop--;
  return *vm.stackTop;
}
void initVM()
{
  resetStack();
}

static InterpretResult run()
{
#define READ_BYTE() (*vm.ip++) // increment IP and get it
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()]) // read a following constants value
  for (;;) //run until we dont want you to run anymore!
  {
    uint8_t instruction; // current instruction
#ifdef DEBUG_TRACE_EXECUTION
    printf("         ");
    for(Value *slot = vm.stack; slot < vm.stackTop; slot++)
    {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    switch(instruction = READ_BYTE())
    {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      }
      case OP_NEGATE: push(-pop()); break; //negate current value on top of the stack and push it directly back onto it

      case OP_RETURN: {
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
      }
    }
  }
#undef READ_CONSTANT
#undef READ_BYTE
}

InterpretResult interpret(Chunk *chunk)
{
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}

void freeVM()
{

}
