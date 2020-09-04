#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char **argv)
{
  // init vm
  initVM();
  // init chunk
  Chunk chunk;
  initChunk(&chunk);
  // create a constant
  int constant = addConstant(&chunk, 1.2);

  // write chunks
  writeChunk(&chunk, OP_CONSTANT,123 );
  writeChunk(&chunk, constant, 123);
  
  
  
    constant = addConstant(&chunk, 3.4);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123);

  writeChunk(&chunk, OP_ADD, 123);

  constant = addConstant(&chunk, 5.6);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123);

  writeChunk(&chunk, OP_DIVIDE, 123);
  
  
  
  writeChunk(&chunk, OP_NEGATE, 123);
  writeChunk(&chunk, OP_RETURN, 123);

  disassembleChunk(&chunk, "test chunk", 0);
  interpret(&chunk); // lets GO! Interpret!
  freeVM();
  freeChunk(&chunk);
  return 0;
}
