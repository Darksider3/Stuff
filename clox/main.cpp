#include "common.h"
#include "chunk.h"
#include "debug.h"


int main(int argc, const char **argv)
{
  // init chunk
  Chunk chunk;
  initChunk(&chunk);
  // create a constant
  int constant = addConstant(&chunk, 1.2);

  // write chunks
  writeChunk(&chunk, OP_RETURN, 123);
  writeChunk(&chunk, OP_CONSTANT,123 );
  writeChunk(&chunk, constant, 123);

  disassembleChunk(&chunk, "test chunk", 0);
  freeChunk(&chunk);
  return 0;
}
