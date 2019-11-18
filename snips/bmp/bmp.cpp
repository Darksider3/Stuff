#include "bmp.h"
#include <iostream>
#include <memory>
#include <typeinfo>
#include <SFML/Graphics.hpp>

#define SCROLLFACTOR 4

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
int main()
{
  DBG << "here";
  std::string fname = "./w3ctest.bmp";
  bmp::Impl T = bmp::Impl(fname);
  T.readData();
  auto data = T.legacyUint8RGBA();
  unsigned int widthpx=static_cast<unsigned int>(T.dib.width_px);
  unsigned int heightpx=static_cast<unsigned int>(T.dib.height_px);
  sf::RenderWindow window(sf::VideoMode(520, 520), "BMP View");
  sf::RectangleShape shape(sf::Vector2(static_cast<float>(window.getSize().x)/2, static_cast<float>(window.getSize().y)/2));
  sf::Image img;
  DBG << widthpx << "x" << heightpx <<":"<<T.DATA.size();
  img.create(widthpx, heightpx, data);
  DBG << img.getSize().x << "x" << img.getSize().y;
  img.flipVertically();
  sf::Texture texture;
  texture.loadFromImage(img);
  DBG << "Texture: " << texture.getSize().x << "x" << texture.getSize().y;
  shape.setTexture(&texture);
  shape.setPosition((window.getSize().x - shape.getSize().x)/2, (window.getSize().y - shape.getSize().y)/2);
  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::MouseWheelScrolled)
      {
        shape.setSize(sf::Vector2(shape.getSize().x+event.mouseWheelScroll.delta*SCROLLFACTOR, shape.getSize().y+event.mouseWheelScroll.delta*SCROLLFACTOR));
      }
      if(event.type == sf::Event::KeyPressed)
      {
        switch(event.key.code)
        {
          case sf::Keyboard::Escape:
            shape.setSize(sf::Vector2(static_cast<float>(window.getSize().x)/2, static_cast<float>(window.getSize().y)/2));
            break;
          case sf::Keyboard::Q:
            return EXIT_SUCCESS;
            break;
          default:
            break;
        }
      }
    }
    shape.setPosition((window.getSize().x - shape.getSize().x)/2, (window.getSize().y - shape.getSize().y)/2);
    window.clear();
    window.draw(shape);
    window.display();
  }
  delete[] data;
}

