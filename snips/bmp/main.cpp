#include <iostream>
#include <memory>
#include <typeinfo>
#include <SFML/Graphics.hpp>
#include <thread>
#include "bmp.h"
#include "textbox.h"


#define SCROLLFACTOR 4
#define ROTATEFACTOR 0.25
#define ROTATE
#undef ROTATE


static sf::Mutex Lock;

#ifdef ROTATE

void AnimateClockWiseRotation(sf::RenderWindow &n)
{
  sf::View changer;
  sf::Time T = sf::milliseconds(25);
  while(true)
  {
    Lock.lock();
    changer = n.getView();
    changer.rotate(-ROTATEFACTOR);
    n.setView(changer);
    Lock.unlock();
    sf::sleep(T);
  }
}
#endif
int main(int argc, char**argv)
{
  auto RotateLeft = [](sf::RenderWindow &n)
  {
    sf::View change = n.getView();
    change.rotate(ROTATEFACTOR);
    n.setView(change);
  };

  auto RotateRight = [](sf::RenderWindow &n)
  {
    sf::View change = n.getView();
    change.rotate(-ROTATEFACTOR);
    n.setView(change);
  };
  std::string fname;
  if(argc > 1)
    fname = argv[1];
  else
    fname = "./pics/zelda.bmp";
  if(!bmp::Impl::exists(fname))
  {
    std::cout << "The file " << fname << " doesn't exist.\n";
    abort();
  }
  bmp::Headers t(fname);
  bmp::Impl T = bmp::Impl(fname);
  T.readData();
  std::string StatText = "";
  if(t.ColorTable())
  {
    StatText+="Colors in Table: "+ std::to_string(t.NumColors()) + "\n";
  }
  StatText += "BPP: " + std::to_string(t.BitPerPixel()) + "\nSize: " + std::to_string(static_cast<int>(t.bmp_header.Size));
  auto data = T.legacyUint8RGBA();
  unsigned int widthpx=static_cast<unsigned int>(T.dib.width_px);
  unsigned int heightpx=static_cast<unsigned int>(T.dib.height_px);
  sf::Vector2u static Windowpx=sf::Vector2u(520, 420);
  sf::RenderWindow window(sf::VideoMode(Windowpx.x, Windowpx.y), "BMP Viewer");
  sf::Vector2f Oldshape = sf::Vector2f(static_cast<float>(window.getSize().x/2), static_cast<float>(window.getSize().y/2));
  sf::RectangleShape shape(Oldshape);
  sf::Image img;

  img.create(widthpx, heightpx, data);
  img.flipVertically();
  sf::Texture texture;
  texture.loadFromImage(img);
  window.setFramerateLimit(60);
#ifdef DEBUG

  sf::Text StatisticalThingy;
  StatisticalThingy.setCharacterSize(15);
  StatisticalThingy.setFillColor(sf::Color::White);
  sf::Font Trara;
  if(!Trara.loadFromFile("/usr/share/fonts/TTF/arial.ttf"))
  {
    DBG << "Couldn't load arial.TTF from /usr/share/fonts/TTF!";
    abort();
  }
  StatisticalThingy.setFont(Trara);
  StatisticalThingy.setPosition(Windowpx.x*0.05f, Windowpx.y*0.9f);
  StatisticalThingy.setString(StatText);

  TextBox Tester;
  sf::Color Grey;
  Tester.create(StatisticalThingy, sf::Vector2f(0,0), sf::Color(156, 140, 140, 120));
  DBG << "Data.size.Byte -> " << T.DATA.size();
  DBG << "ImageX:ImageY -> " << img.getSize().x << "x" << img.getSize().y;
  DBG << "Texturesize: " << texture.getSize().x << "x" << texture.getSize().y;
#endif
  shape.setTexture(&texture);
  auto shapeMid = [&]()
  {
    return sf::Vector2f((window.getSize().x - shape.getSize().x)/2, (window.getSize().y - shape.getSize().y)/2);
  };
  //shape.setPosition(shapeMid());
#ifdef ROTATE
  sf::Thread Threader(&AnimateClockWiseRotation, std::ref(window));
  Threader.launch();
#endif
  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::MouseWheelScrolled)
      {
        shape.setSize(sf::Vector2f(shape.getSize().x+event.mouseWheelScroll.delta*SCROLLFACTOR,
                                   shape.getSize().y+event.mouseWheelScroll.delta*SCROLLFACTOR));
        shape.setPosition(shapeMid());
      }
      if(event.type == sf::Event::Resized)
      {
        // when the window is resized, the mappings get out of scope. Remap them to our current X,Y coords
        window.setView(sf::View(sf::FloatRect(0,0, event.size.width, event.size.height)));
        shape.setPosition(shapeMid());
        shape.setOrigin(5,5);
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
            shape.setSize(sf::Vector2f(img.getSize().x/2, img.getSize().y/2));
            shape.setPosition(shapeMid());
            shape.setOrigin(5,5);
            break;
          case sf::Keyboard::Left:
            RotateLeft(window);
            break;
          case sf::Keyboard::Right:
            RotateRight(window);
            break;
          default:
            break;
        }
      }
    }
    Lock.lock();
    window.clear();
    window.draw(shape);
#ifdef DEBUG
    window.draw(Tester);
#endif
    window.display();
    Lock.unlock();
  }
#ifdef ROTATE
  Threader.terminate();
#endif
  delete[] data;
}
