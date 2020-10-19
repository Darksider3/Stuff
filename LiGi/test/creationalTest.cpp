#include "../creational.h"
#include <iostream>

class testthingy : public Li::creational::construct_weak<testthingy> {
public:
    void echoThingy()
    {
        std::cout << "was here";
    }
};

class testthingy2 : public Li::creational::construct_unique<testthingy2> {
public:
    explicit testthingy2(std::string const& text, std::string const& text2)
    {
        std::cout << text << text2;
    }
};

int main()
{
    auto blblbl = testthingy2::create("Hello World!", "I was here!");
    auto bla = testthingy::create().lock();
    bla->echoThingy();

    if (testthingy::is_plainptr_constructible) {
    }
}
