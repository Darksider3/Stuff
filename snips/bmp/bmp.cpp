#include "bmp.h"
#include <iostream>
#include <memory>
#include <typeinfo>
#include <SFML/Graphics.hpp>

bmp::Impl::Impl(std::string &FN)
{
  if(!exist(FN))
  {
    std::cout << "The file " << FN << " doesn't exist... \n";
    abort();
  }
  if(std::filesystem::file_size(FN) < 54)
  {
    std::cout << FN << " is too small for a bmp file.";
    abort();
  }
  f.open(FN, std::ios::binary);
  readHeaders();
  if(!check_header())
  {
    std::cout << "something went wrong! Not a BMP-File";
    abort();
  }
  else
  {
    DBG << "it's a BMP, be happy!";
  }
  paddingBytes = dib.width_px % 4;
}

void bmp::Impl::readHeaders()
{
  f.read(reinterpret_cast<char*>(&header), sizeof(BitmapFileHeader));
  f.read(reinterpret_cast<char*>(&dib), sizeof(DIB_BITMAPINFOHEADER));
}
void bmp::Impl::readData()
{
  Pixel_BGR24 tmp;
  uint8_t trash;
  f.seekg(header.Offset);
  for(long long int i = 0; i != dib.height_px; ++i)
  {
    for(long long int y = 0; y != dib.width_px; ++y)
    {
      f.read(reinterpret_cast<char*>(&tmp), sizeof(tmp));
      DATA.push_back(tmp);
    }
    if(!(paddingBytes == 0))
    {
      for(int x = paddingBytes; x != 0; --x)
        f.read(reinterpret_cast<char*>(&trash), paddingBytes);
      DBG << "trashing " << paddingBytes << " padding bytes";
    }
  }
  DBG << "Read done!";
  f.close();
}

bool bmp::Impl::check_header()
{
  if(header.Type != 0x4d42) // BM header
  {
    return false;
  }
  if(header.dib_header_size != 40)
  {
    std::cout << "not supported! DIB-Header != 40\n";
    abort();
  }
  return true;
}
std::shared_ptr<uint8_t[]> bmp::Impl::getDataSmartUint8RGBA()
{
  std::shared_ptr<uint8_t[]> returner = std::make_shared<uint8_t[]>((DATA.size())*4);
  std::ptrdiff_t i = 0;
  for(size_t y = 0; y != DATA.size(); ++y)
  {
    returner[i] = DATA[y].R;
    ++i;
    returner[i] = DATA[y].G;
    ++i;
    returner[i] = DATA[y].B;
    ++i;
    returner[i] = 255; // A
    ++i;
  }
  return returner;
}

uint8_t *bmp::Impl::legacyUint8RGBA()
{
  DBG << "Data:  " << DATA.size()*4;
  uint8_t *returner = new uint8_t[(DATA.size()+1)*4];
  size_t i = 0;
  size_t y = 0;
  for(; y < DATA.size(); ++y)
  {
    returner[i] = DATA[y].R;
    ++i;
    returner[i] = DATA[y].G;
    ++i;
    returner[i] = DATA[y].B;
    ++i;
    returner[i] = 255; // A
    ++i;
  }
  DBG << "Data: " << i;
  DBG << "Data: " << y*4;
  return returner;

  //return getDataSmartUint8RGBA().get();
}

bmp::Impl::~Impl()
{
  if(f.is_open())
    f.close();
}

