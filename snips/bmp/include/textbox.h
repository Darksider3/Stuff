#ifndef TEXTBOX_H
#define TEXTBOX_H
#include  <SFML/Graphics.hpp>

class TextBox : public sf::Drawable, public sf::Transformable
{
public:
  TextBox create(sf::Text text, sf::Vector2f position, sf::Color RectColor);
  void complementary_color_fast_approx(sf::Color const &);

  void setTextColor(sf::Color const &color)
  {
    m_text.setFillColor(color);
  }

protected:
  sf::RectangleShape m_box;
  sf::Text m_text;

  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
  {
    target.draw(m_box, states);
    target.draw(m_text, states);
  }
};

#endif // TEXTBOX_H
