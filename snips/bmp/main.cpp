#include "bmp.h"
#include <iostream>
#include <memory>
#include <typeinfo>
#include <SFML/Graphics.hpp>

#define SCROLLFACTOR 4


int main()
{
  DBG << "here";
  std::string fname = "./zelda.bmp";
  bmp::Headers t(fname);
  bmp::Impl T = bmp::Impl(fname);
  T.readData();
  auto data = T.legacyUint8RGBA();
  unsigned int widthpx=static_cast<unsigned int>(T.dib.width_px);
  unsigned int heightpx=static_cast<unsigned int>(T.dib.height_px);
  sf::Vector2u static Windowpx=sf::Vector2u(520, 520);
  sf::RenderWindow window(sf::VideoMode(Windowpx.x, Windowpx.y), "BMP View");
  sf::Vector2f Oldshape = sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
  sf::RectangleShape shape(Oldshape);
  sf::Image img;
  img.create(widthpx, heightpx, data);
  img.flipVertically();
  sf::Texture texture;
  texture.loadFromImage(img);
  DBG << "Data.size.Byte -> " << T.DATA.size();
  DBG << "ImageX:ImageY -> " << img.getSize().x << "x" << img.getSize().y;
  DBG << "Texturesize: " << texture.getSize().x << "x" << texture.getSize().y;
  shape.setTexture(&texture);
  auto shapeMid = [&]()
  {
    return sf::Vector2f((window.getSize().x - shape.getSize().x)/2, (window.getSize().y - shape.getSize().y)/2);
  };
  shape.setPosition(shapeMid());
  while (window.isOpen())
  {
    shape.scale(1,1);
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::MouseWheelScrolled)
      {
        shape.setSize(sf::Vector2f(shape.getSize().x+event.mouseWheelScroll.delta*SCROLLFACTOR, shape.getSize().y+event.mouseWheelScroll.delta*SCROLLFACTOR));
        shape.setPosition(shapeMid());
      }
      if(event.type == sf::Event::Resized)
      {
        // when the window is resized, the mappings get out of scope. Remap them to our current X,Y coords
        window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
        shape.setPosition(shapeMid());
      }
      if(event.type == sf::Event::KeyPressed)
      {
        switch(event.key.code)
        {
          case sf::Keyboard::Escape:
            window.setSize(Windowpx);
            window.setView(sf::View(sf::FloatRect(0, 0, Windowpx.x, Windowpx.y)));
            shape.setSize(Oldshape);
            shape.setPosition(shapeMid());
            window.requestFocus();
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
    window.clear();
    window.draw(shape);
    window.display();
  }
  delete[] data;
}
