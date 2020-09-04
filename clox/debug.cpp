#include <cstdio>

#include "debug.h"
#include "value.h"

/**
 * @brief disassembleChunk follows the specified chunk and prints out all available information about it
 * @param chunk
 * @param name
 * @param offset
 */
void disassembleChunk(Chunk *chunk, const char *name, int offset = 0)
{
  printf("== %s ==\n", name);
  
  for(; offset < chunk->count;)
  {
    offset = disassembleInstruction(chunk, offset);
  }
}
/*
 * Prints OP-Codes(which are effetivly 1-Byte, e.g. OP_RETURN)
 */
static int simpleInstruction(const char* name, int offset)
{
  printf("%s\n", name);
  return offset + 1;
}


/*
 * Print out constant instructions
 */
static int constantInstruction(const char* name, Chunk *chunk, int offset)
{
  uint8_t constant = chunk->code[offset +1];
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  // two-byte instruction actually(OP_CODE(1)+CONSTANT(2))
  return offset + 2;
}

/**
 * @brief disassembleInstruction prints a chunk for debug information, at specific offset
 * @param chunk
 * @param offset
 * @return
 */
int disassembleInstruction(Chunk *chunk, int offset)
{
  printf("%04d ", offset);
  // when the line info is the same in two different chunks, just print out
  // a continuation
  if(offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1])
  {
    printf("   | ");
  }
  else
  {
    printf("%4d ", chunk->lines[offset]);
  }
  uint8_t instruction = chunk->code[offset];
  
  switch(instruction)
  {
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_ADD:
      return simpleInstruction("OP_ADD", offset);
    case OP_SUBTRACT:
      return simpleInstruction("OP_SUBTRACT", offset);
    case OP_DIVIDE:
      return simpleInstruction("OP_DIVIDE", offset);
    case OP_MULTIPLY:
      return simpleInstruction("OP_MULTIPLY", offset);
    case OP_NEGATE:
      return simpleInstruction("OP_NEGATE", offset);
    default:
      printf("Unknown opcode %d\n", instruction);
      return offset + 1;
  }
}
