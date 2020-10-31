#include <cstdio>
#include <stdarg.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"

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

static void runtimeError(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);

    resetStack();
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

static Value peek(int distance)
{
    return vm.stackTop[-1 - distance];
}

static bool isFalsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
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
#define BINARY_OP(valueType, op)                          \
    do {                                                  \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operands must be numbers.");    \
            return INTERPRET_RUNTIME_ERROR;               \
        }                                                 \
        double b = AS_NUMBER(pop());                      \
        double a = AS_NUMBER(pop());                      \
        push(valueType(a op b));                          \
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
        
        case OP_NIL: push(NIL_VAL); break;
        case OP_TRUE: push(BOOL_VAL(true)); break;
        case OP_FALSE: push(BOOL_VAL(false)); break;

        // BINARY OPs:
        case OP_ADD:
            BINARY_OP(NUMBER_VAL, +);
            break;
        case OP_SUBTRACT:
            BINARY_OP(NUMBER_VAL, -);
            break;
        case OP_MULTIPLY:
            BINARY_OP(NUMBER_VAL, *);
            break;
        case OP_DIVIDE:
            BINARY_OP(NUMBER_VAL, /);
            break;
        
        case OP_NOT:
            push(BOOL_VAL(isFalsey(pop())));
            break;

        case OP_NEGATE:
            if (!IS_NUMBER(peek(0))) {
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }

            push(NUMBER_VAL(-AS_NUMBER(pop())));
            break;

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
