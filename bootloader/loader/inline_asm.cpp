#include <cstdint>
#include <iostream>
#define DB uint8_t
#define DW uint16_t
#define DD uint32_t
#define DQ uint64_t

extern "C" struct gdt_start
{
  DQ one = 0x00;
};

extern "C" struct gdt_code
{
  DQ two = 0xFFFF;
  DW three  = 0;
  DB four = 0;
  DB five = 0x9A; // 10011010b
  DB six = 0xCF; // 11001111b
};

int main()
{
  gdt_start bla;
  gdt_code blub;
  DQ tester = 1;

  asm ("mov %1, %0;"
       "mov %%rax, %1"
      : "=r" (bla.one)
      : "r" (tester));
  asm ("mov %%rax, %0;"
      : "=r" (blub.two)
      : "r" (tester)
      : "%rax");

  std::cout << "* " << bla.one << "\n* " << blub.two << "\n";
  return 0;
}
