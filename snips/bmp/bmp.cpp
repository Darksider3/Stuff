#include "bmp.h"
#include <iostream>
bmp::Impl::Impl(std::string &FN)
{
  if(!exist(FN))
  {
    std::cout << "The file " << FN << " doesn't exist... \n";
    abort();
  }
  if(std::filesystem::file_size(FN) < 54)
  {
    std::cout << FN << " is too small for a bmp file.\n";
    abort();
  }
  f.open(FN, std::ios::binary);
  readHeader();
  if(!check_header())
  {
    std::cout << "something went wrong! Not a BMP-File\n";
    abort();
  }
  else
  {
    std::cout << "it's a BMP, be happy!\n";
  }
  paddingBytes = header.width_px % 4;
}

void bmp::Impl::readHeader()
{
  f.read(reinterpret_cast<char*>(&header), sizeof(BitmapFileHeader));
}
void bmp::Impl::readData()
{
  BGR_8 tmp;
  char trash;
  f.seekg(header.Offset);
  for(long long int i = 0; i < header.height_px; ++i )
  {
    for(long long int y = 0; y < header.height_px; ++y)
    {
      f.read(reinterpret_cast<char*>(&tmp), sizeof(tmp));
      values.push_back(tmp);
    }
    if(paddingBytes > 0)
    {
      f.read(&trash, paddingBytes);
      std::cout << "trashing " << paddingBytes << " padding bytes\n";
    }
  }
  std::cout << "Read done!\n";
}

bool bmp::Impl::check_header()
{
  if(header.Type == 0x4d42) // BM header
  {
    return true;
  }
  return false;
}

bmp::Impl::~Impl()
{
  if(f.is_open())
    f.close();
}
int main()
{
  std::string fname = "./zelda.bmp";
  bmp::Impl T = bmp::Impl(fname);
  std::cout.flush();
  T.readData();
}

