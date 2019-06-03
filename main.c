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

  src[i] = 0; // add EOF
  close(fd);

  program();
  return eval();
}