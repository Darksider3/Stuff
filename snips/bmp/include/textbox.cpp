#include <SFML/Graphics.hpp>
#include <iostream>
#include "textbox.h"

TextBox TextBox::create(sf::Text text, sf::Vector2f position, sf::Color RectColor)
{
  sf::FloatRect Boundings;
  Boundings = text.getLocalBounds();
  sf::Vector2f MIN_NEED_SIZES(Boundings.width+(Boundings.width*0.1f), Boundings.height+(Boundings.height*0.2f));
  m_text = text;
  m_text.setFillColor(sf::Color::Black);
  m_box = sf::RectangleShape(MIN_NEED_SIZES);
  m_box.setFillColor(RectColor);
  m_box.setPosition(position);
  m_text.setPosition(position);
  complementary_color_fast_approx(m_text.getFillColor());
  return *this;
}

void TextBox::complementary_color_fast_approx(const sf::Color &t)
{
  sf::Color tmp(t);
  tmp.r ^= 0xfff;
  tmp.g ^= 0xfff;
  tmp.b ^= 0xfff;
  std::cout << std::to_string(tmp.a) << " " << std::to_string(tmp.r) << " " << std::to_string(tmp.g) << " " << std::to_string(tmp.b) << " " << "\n";
  m_text.setFillColor(tmp);
}
