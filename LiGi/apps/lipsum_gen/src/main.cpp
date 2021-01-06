//
// Created by darksider3 on 06.01.21.
//
#include "lipsum.hpp"
#include <fstream>
#include <iostream>

constexpr char banner[] = "\n"
                          "\n"
                          "                                                             \n"
                          "                                                             \n"
                          "        #                                                    \n"
                          "###                                          ##              \n"
                          " #     ##   # ##   ### ## ##  ## #          #  #  ##  # ##   \n"
                          " #      #    #  # #     #  #  # # #         #    #  #  #  #  \n"
                          " #      #    #  #  ##   #  #  # # #         # ## ###   #  #  \n"
                          " #  #   #    #  #    #  #  #  # # #         #  # #     #  #  \n"
                          "##### #####  ###  ###    ## # # # ##         ##   ### ### ## \n"
                          "             #                                               \n"
                          "            ###                                              \n"
                          "Version: 0.1.4"
                          "\n\n\n";

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

void help(char** args)
{
    std::cout << banner;
    std::cout << "Usage: " << args[0] << " <size(in bytes) to generate>\n"
              << std::endl;
    std::exit(0);
}

int main(int argc, char** argv)
{
    if (argc < 2)
        help(argv);

    if (!is_number(std::string(argv[1])))
        help(argv);

    std::size_t out = std::stoul(std::string(argv[1]));
    lipsum g { out };
    std::cout << g << std::endl;
}