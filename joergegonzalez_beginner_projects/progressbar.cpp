#include <iostream>
#include <chrono>
#include <thread>
void Draw(int len, double percent)
{
  // std::cout << "\x1B[2K"; // Erase the enitre current line
  // Does text get cleared
  std::cout << "Test";
  std::flush(std::cout);
  using namespace std::chrono_literals;
  // sleep for 1s so that we can verify it works by sight
  std::this_thread::sleep_for(1s);
  std::cout << "\b"; // ???
  std::cout << "\r"; // go to the beginning of the line
  std::cout << "\033[K"; // clear the actual line
  std::string progress;

  for(int i = 0; i != len; ++i)
  {
    if(i < static_cast<int>(len*percent))
    {
      progress += '=';
    }
    else
    {
      progress += ' ';
    }
  }

  std::cout << "[" <<  progress << "] " << (static_cast<int>(100*percent)) << "%";
  std::flush(std::cout); //Required
}

int main()
{
  for(double i = 0; i != 1; i+=0.01)
  {
    using namespace std::chrono_literals;
    Draw(80, i);
    std::this_thread::sleep_for(1s);
  }
  std::cout << std::endl;
  return 0;
}
