#include <sys/types.h> // <-
#include <sys/stat.h> // <- Fifo
#include <unistd.h> // write
#include <fcntl.h>
#include <iostream>
#include <filesystem>
#include <stdio.h>
namespace fifo
{

}

int main()
{
  namespace fs = std::filesystem;
  fs::path fifoFile="./fifo";
  int fd;
  if(fs::exists(fifoFile))
  {
    std::cout << "Fifo already exists, we use it. Risky?\n";
  }
  else
  {
    if(mkfifo(fifoFile.c_str(), 0777))
    {
      std::cout << "fifo couldnt be created";
      return EXIT_FAILURE;
    }
  }
  fd = open(fifoFile.c_str(), O_RDWR | O_NONBLOCK);
  if(fd == -1)
  {
    perror("WROOONG");
    return EXIT_FAILURE;
  }



  for(int i = 0; i != 5; ++i)
  {
    write(fd, "asd", 4);
    sleep(2);
  }
  write(fd, "end", 4);
  close(fd);
  fs::remove(fifoFile);
  return 0;
}
