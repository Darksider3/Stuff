//
// Created by darksider3 on 22.12.20.
//

#ifndef LIGI_APPS_CONCRETESTATUSES_H
#define LIGI_APPS_CONCRETESTATUSES_H
#include "AbstractStatus.h"
#include <string>

/**
 * @brief The Status200 class implements Status Code 200
 */
class Status200 : public AbstractStatus {
public:
    [[nodiscard]] std::string get() const override { return "200 OK"; }
};

/**
 * @brief The Status404 class implements Status Code 404
 */
class Status404 : public AbstractStatus {
public:
    [[nodiscard]] std::string get() const override { return "404 Not Found"; }
};
#endif //LIGI_APPS_CONCRETESTATUSES_H
