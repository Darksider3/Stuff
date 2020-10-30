#include "vm.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include <cstdio>

class VirtualMachine {
private:
    Chunk* chunk;
    uint8_t* ip; // instruction pointer, we could also call it PC for program counter
    Value stack[STACK_MAX];
    Value* stackTop;

public:
    explicit VirtualMachine(Chunk* chunk)
    {
        this->reset();
        this->chunk = chunk;
    }

    void reset()
    {
        stackTop = stack;
    }

    void push(Value val)
    {
        *this->stackTop = val;
        ++this->stackTop;
    }

    Value pop()
    {
        --this->stackTop;
        return *stackTop;
    }

    InterpretResult run();
};
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
    vm.stackTop++;        // and advance it
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

#define READ_BYTE() (*vm.ip++) // increment IP and get it || @TODO: Rename to something sane - READ_IP()?

#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()]) // read a following constants value

#define NEGATE_PUSH_POP() (push(-pop()))
#define BINARY_OP(op)     \
    do {                  \
        double b = pop(); \
        double a = pop(); \
        push(a op b);     \
    } while (false)

    for (;;) //run until we dont want you to run anymore!
    {
        uint8_t instruction; // current instruction
#ifdef DEBUG_TRACE_EXECUTION
        printf("         ");
        for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        switch (instruction = READ_BYTE()) {
        case OP_CONSTANT: {
            Value constant = READ_CONSTANT();
            push(constant);
            break;
        }

        // BINARY OPs:
        case OP_ADD:
            BINARY_OP(+);
            break;
        case OP_SUBTRACT:
            BINARY_OP(-);
            break;
        case OP_MULTIPLY:
            BINARY_OP(*);
            break;
        case OP_DIVIDE:
            BINARY_OP(/);
            break;

        case OP_NEGATE:
            push(-pop());
            break; //negate current value on top of the stack and push it directly back onto it

        case OP_RETURN: {
            printValue(pop());
            printf("\n");
            return INTERPRET_OK;
        }
        }
    }
#undef READ_CONSTANT
#undef READ_BYTE
#undef BINARY_OP
}

InterpretResult interpret(const char* source)
{
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}

void freeVM()
{
}
