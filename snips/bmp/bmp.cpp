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
  readHeader();
  if(!check_header())
  {
    std::cout << "something went wrong! Not a BMP-File";
    abort();
  }
  else
  {
    DBG << "it's a BMP, be happy!";
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
  for(long long int i = 0; i != header.height_px; ++i )
  {
    for(long long int y = 0; y != header.width_px; ++y)
    {
      f.read(reinterpret_cast<char*>(&tmp), sizeof(BGR_8));
      DATA.push_back(tmp);
    }
    if(!(paddingBytes == 0))
    {
      f.read(&trash, paddingBytes);
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
  std::shared_ptr<uint8_t[]> returner = std::make_shared<uint8_t[]>((DATA.size())*32);
  std::ptrdiff_t i = 0;
  for(size_t y = 0; y != DATA.size()-1; ++y)
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
  uint8_t *returner = new uint8_t[(DATA.size())*32];
  for(size_t i = 0, y=0; y != DATA.size(); ++y)
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

  //return getDataSmartUint8RGBA().get();
}

bmp::Impl::~Impl()
{
  if(f.is_open())
    f.close();
}
int main()
{
  sf::RenderWindow window(sf::VideoMode(1024, 1024), "BMP View");
  DBG << "here";
  sf::RectangleShape shape(sf::Vector2(513.f, 513.f));
  sf::Image img;
  std::string fname = "./zelda.bmp";
  bmp::Impl T = bmp::Impl(fname);
  T.readData();
  auto data = T.legacyUint8RGBA();
  img.create(static_cast<unsigned int>(T.header.width_px), static_cast<unsigned int>(T.header.height_px), data);
  DBG << img.getSize().x << "x" << img.getSize().y;
  img.flipVertically();
  sf::Texture texture;
  texture.loadFromImage(img);
  shape.setTexture(&texture);
  while (window.isOpen())
  {
      sf::Event event;
      while (window.pollEvent(event))
      {
          if (event.type == sf::Event::Closed)
              window.close();
      }

      window.clear();
      window.draw(shape);
      window.display();
  }
}

