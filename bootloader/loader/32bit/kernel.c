#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
  // clear VGA textmode screen
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) 
{
  if(c == '\n')
  {
    ++terminal_row;
    terminal_column = 0;
    return;
  }
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
  }
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

char integerbuff[256];

void IntegertoString(int number) {

   if(number == 0) {
     integerbuff[0] = '0';
     return;
   }
   int divide = 0;
   int modResult;
   int  length = 0;
   int isNegative = 0;
   int  copyOfNumber;
   int offset = 0;
   copyOfNumber = number;
   if( number < 0 ) {
     isNegative = 1;
     number = 0 - number;
     length++;
   }
   while(copyOfNumber != 0)
   {
     length++;
     copyOfNumber /= 10;
   }

   for(divide = 0; divide < length; divide++) {
     modResult = number % 10;
     number    = number / 10;
     integerbuff[length - (divide + 1)] = modResult + '0';
   }
   if(isNegative) {
   integerbuff[0] = '-';
   }
   integerbuff[length] = '\0';
}

struct registers {
  intptr_t eax;
  intptr_t ebx;
  intptr_t ecx;
  intptr_t edx;
} GP_REGS;

void read_reg(char *name, intptr_t *dest)
{
  __asm__("movl %%%0, %1"
          : "=r" (name)
          : "r" (dest));
}


void read_registers()
{
  __asm__("movl %%eax, %0"
      :"=r" (GP_REGS.eax));
  read_reg("ebx", &GP_REGS.ebx);
}

void print_reg(intptr_t *Reg, char *name)
{
  IntegertoString(*Reg);
  terminal_writestring(name);
  terminal_writestring(": ");
  terminal_writestring(integerbuff);
  terminal_writestring("\n");
}

void read_test()
{
  __asm__ volatile("movl $0x0101, %edx");
  intptr_t edxtest;
  __asm__("movl %%edx, %0":
          "=rr" (edxtest));
  print_reg(edxtest, "edxtest");
  __asm__ ("movl %eax, %edx");
  __asm__ ("movl %%edx, %0":
          "=r" (GP_REGS.edx));
  print_reg(GP_REGS.edx, "edx");
}

void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
  terminal_setcolor(VGA_COLOR_RED);
  read_registers();

  read_test();
  GP_REGS.eax = 300;
	/* Newline support is left as an exercise. */
  terminal_writestring("Hello, kernel World!\n");
  terminal_writestring("Not really a deal!\n");
  terminal_writestring("EAX should be 300: ");
  print_reg(&GP_REGS.eax, "eax");
  print_reg(&GP_REGS.ebx, "ebx");
  GP_REGS.ebx = 123;
  print_reg(&GP_REGS.ebx, "ebx");
  GP_REGS.ebx = 0x001;
}
