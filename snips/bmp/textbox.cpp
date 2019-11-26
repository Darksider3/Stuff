#include <SFML/Graphics.hpp>
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
  return *this;
}
