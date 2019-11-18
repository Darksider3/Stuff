#include "bmp.h"
#include <iostream>
#include <memory>
#include <typeinfo>
#include <SFML/Graphics.hpp>

#define SCROLLFACTOR 4


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
  sf::RectangleShape shape(sf::Vector2f(static_cast<float>(window.getSize().x)/2, static_cast<float>(window.getSize().y)/2));
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
        shape.setSize(sf::Vector2f(shape.getSize().x+event.mouseWheelScroll.delta*SCROLLFACTOR, shape.getSize().y+event.mouseWheelScroll.delta*SCROLLFACTOR));
      }
      if(event.type == sf::Event::KeyPressed)
      {
        switch(event.key.code)
        {
          case sf::Keyboard::Escape:
            shape.setSize(sf::Vector2f(static_cast<float>(window.getSize().x)/2, static_cast<float>(window.getSize().y)/2));
            break;
          case sf::Keyboard::Q:
            return EXIT_SUCCESS;
          case sf::Keyboard::Space:
            shape.setSize(sf::Vector2f(img.getSize().x, img.getSize().y));
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
