#include <iostream>

bool is_pythagorean_triple(size_t x, size_t y, size_t z)
{
  return (x*x + y*y == z*z);
}

int main()
{
  bool run = true;
  size_t A, B, C;
  std::string inputbuf;

  std::cin.clear();

  while(run)
  {
    std::cout << "Input your first number: ";
    std::cin >> inputbuf;
    
    A = std::stoi(inputbuf);
    std::cin.clear();
    inputbuf = "";

    std::cout << "\nInput secondn number: ";
    std::cin  >> inputbuf;
    
    B = std::stoi(inputbuf);
    std::cin.clear();
    inputbuf = "";

    std::cout << "\nInput third number: ";
    std::cin >> inputbuf;
    
    C = std::stoi(inputbuf);
    std::cin.clear();
    inputbuf = "";

    if(is_pythagorean_triple(A, B, C))
      std::cout << "It is an pythagorean triple!\n";
    else
      std::cout << "Saadly it isnt...\n";

    std::fflush(stdout);
  }
}
