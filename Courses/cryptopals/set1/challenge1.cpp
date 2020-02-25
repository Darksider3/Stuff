#include <iostream>
#include <sstream>
#include "base64_encode.h"
#include <stdlib.h>
#include <bitset>
static char b64table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz"
                           "0123456789+/";

std::string hexify(unsigned int n)
{
  std::string res;

  do
  {
    res += "0123456789ABCDEF"[n % 16];
    n >>= 4;
  } while(n);

  return std::string(res.rbegin(), res.rend());
}

int main()
{
  std::stringstream ss;
  std::string tmp;
  std::cout << "Convert following hex: ";
  std::cin >> tmp;
  ss << std::hex <<tmp;
  tmp = ss.str();
  std::cout << "ss: " << ss.str() << "\n";
  std::cout << "tmp: " << tmp << "\n";
  std::string back = base64_encode((const unsigned char*)ss.str().c_str(), ss.str().length());
  std::cout << "Yields to: " << back << "\n";
  std::cout << "-----------------\n";
  ss = std::stringstream();
  std::string bla="49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
  std::stringstream out;
  out << std::hex << bla;
  unsigned long long int n;
  out >> n;
  std::cout << std::to_string(n) << "\n";

}
