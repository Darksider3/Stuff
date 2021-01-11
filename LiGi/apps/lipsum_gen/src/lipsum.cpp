//
// Created by darksider3 on 06.01.21.
//

#include "lipsum.hpp"
#include <string>

std::string&& Lipsum::lipsum::getBytes(std::size_t size, std::string&& in) const
{
    in.reserve(size);
    size_t wholes = size / LipsumData.size();
    size_t LeftOver = size % LipsumData.size();
    for (size_t i = 0; i < wholes; ++i) {
        in.append(LipsumData);
    }

    in.append(LipsumData.begin(), LipsumData.begin() + LeftOver);

    return std::move(in);
}
