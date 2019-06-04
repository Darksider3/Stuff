#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int token;           // current token
char *src, *oldsrc;  // pointer to source code in string(s)
int poolsize;        // default size of  text/data/stack
int line;            // (current?) line number

int *text, *oldtext, *stack; // Text segment, Stack segment

char *data; // data segment

//instructions

/**
 * MOV: Moves data into registers or memory.
 * IMM <num>: to put immediate <num> into register AX
 * LC: to load a character into AX from a memory address which is stored in AX before executeion
 * LI: is just like LC but dealing with integers instead of characters
 * SC: store the character in AX into memory whose address is stored on the top of the stack
 * SI: just like SC but dealing with integer instead of character
 */
enum { LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,
  OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
  OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT };

/**
 * All of this registers are just a place to store values
 * PC: Programmcounter, stores an memory address in which stores the next instruction to be run
 * SP: Stackpointer, which always points to the TOP of the stack. Notice the stack grows from high addresses to low
 * address so that when we push a new element to the stack, SP decreases.
 * BP: basepointer, points to some elements on the stack, it is used in function calls.
 * AX: a general register we used to store the result of an instruction.
 */
int *pc, *bp, *sp, ax, cycle; // Virtual machine registers

void next()
{
  token = *src++;
  return;
}

void expression(int level)
{
  //nothing
}

void program()
{
  next(); //get next token
  while (token > 0 )
  {
    printf("Token is: %c\n", token);
    next();
  }
}

int eval() // do nothing yet
{
  int op, *tmp;
  while(1)
  {
    if(op == IMM) { ax = *pc++;}  // load immediate <num> into AX
    else if(op == LC) { ax = *(char *) ax;} // load character into AX, address is ax
    else if(op == LI) { ax = *(int *) ax;} // load integer into AX, address is ax
    else if(op == SC) { ax = *(char *) *sp++ = ax;} // Save character to address, value in ax, address on stack
    else if(op == SI) { ax = *(int *) *sp++ = ax;} // Save integer to address, value in ax, address on stack
    else if(op == PUSH) { *--sp = ax;} // push the value of ax onto the stack
    else if(op == JMP) { pc = (int *)*pc;} // jmp to address
    else if(op == JZ) { pc = ax ? pc + 1: (int *)*pc} // jump if ax is zero
    else if(op == JNZ){ pc = ax ? (int *)*pc: pc +1;} // jump if ax is nonzero
  }
  return 0;
}

int main(int argc, char **argv)
{
  int i, fd;
  argc--; // dont count our program please...
  argv++; // didnt i just say that?

  poolsize = 256 * 1024; // arbitrary size
  line=1;
  if((fd = open(*argv, 0)) < 0)
  {
    printf("couldnt open(%s)\n", *argv);
    return -1;
  }

  if (!(src = oldsrc = malloc(poolsize)))
  {
    printf("Couldnt malloc(%d) for source area!\n", poolsize);
    return -1;
  }

  if((i = read(fd, src, poolsize-1)) <= 0)
  {
    printf("read() returned %d\n", i);
    return -1;
  }

  if(!(text = oldtext = malloc(poolsize)))
  {
    printf("couldnt malloc(%d) for text area\n", poolsize);
    return -1;
  }

  if(!(data = malloc(poolsize)))
  {
    printf("Couldnt malloc(%d) for data area\n", poolsize);
    return -1;
  }

  if(!(stack = malloc(poolsize)))
  {
    printf("Could not malloc(%d) for stack area\n", poolsize);

    return -1;
  }

  memset(text, 0, poolsize);
  memset(data, 0, poolsize);
  memset(stack, 0, poolsize);

  bp = sp = (int *)((int) stack +poolsize);
  ax = 0;


  src[i] = 0; // add EOF
  close(fd);

  program();
  return eval();
}