#ifndef TEXTBOX_H
#define TEXTBOX_H
#include  <SFML/Graphics.hpp>

class TextBox : public sf::Drawable, public sf::Transformable
{
public:
  TextBox create(sf::Text text, sf::Vector2f position, sf::Color RectColor);
private:
  sf::Text m_text;
  sf::RectangleShape m_box;
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(m_box, states);
    target.draw(m_text, states);
  }
};

#endif // TEXTBOX_H
