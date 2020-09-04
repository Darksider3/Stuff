#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

/*
 */

typedef enum {
  OP_CONSTANT,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NEGATE,
  OP_RETURN, // return from current function
} OpCode;


typedef struct {
  int count;
  int capacity;
  uint8_t *code;
  int* lines; // @TODO: just write the line when actually on a new one; needs to refit all code effectivly writing to the lines parameter
  ValueArray constants;
} Chunk;

/**
 * @brief Initialise a chunk, make it effectivly ready for use
 * @param chunk
 */
void initChunk(Chunk* chunk);
/**
 * @brief deallocate a chunk *and* NULL it
 * @param chunk
 */
void freeChunk(Chunk* chunk);
/**
 * @brief Interface to write a chunk
 * @param chunk
 * @param byte
 * @param line
 */
void writeChunk(Chunk* chunk, uint8_t byte, int line);
/**
 * @brief Add a constant value to the chunk-list
 * @param chunk
 * @param value
 * @return
 */
int addConstant(Chunk *chunk, Value value);
#endif
