//
// Created by darksider3 on 27.12.20.
//

#ifndef LIGI_APPS_CONCRETECONTENTTYPES_H
#define LIGI_APPS_CONCRETECONTENTTYPES_H
#include "AbstractContentType.h"

class HTMLType final : public AbstractContentType {
    [[nodiscard]] std::string get() const final
    {
        return "text/html";
    }
};

#endif //LIGI_APPS_CONCRETECONTENTTYPES_H
