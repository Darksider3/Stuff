#include <iostream>

#ifdef PRINT
#define DEBUG 1
#endif
#ifdef DEBUG
#include <cstdio>
#define DEBUG_PRINT(fmt, args...)    fprintf(stderr, fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)    /* Dont do anything at all pls*/
#endif

int fibonacci_recursive(size_t n)
{
  if(n <= 1)
    return n;
  DEBUG_PRINT(n);
  return fibonacci_recursive(n-1) + fibonacci_recursive(n-2);
}

int fibonacci_iterative(size_t n)
{
  size_t a = 0, b = 1, c, i;
  if(n == 0)
    return a;
  for(i = 2; i <= n; i++)
  {
    c = a +b;
    a = b;
    b = c;
    DEBUG_PRINT(b);
  }
  return b;
}

int main()
{
  std::string temp;
  size_t N;
  std::cout << "Which number in the Fibonacci sequence you wish to see, Senpai? ";
  std::cin >> temp;
  N = std::stoi(temp);
  std::cout << "Senpai! I've found it! The " << N <<"th number is " << fibonacci_iterative(N) << "!! \n";

  return 0;
}
