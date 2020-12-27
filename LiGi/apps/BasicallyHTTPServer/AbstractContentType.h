//
// Created by darksider3 on 27.12.20.
//

#ifndef LIGI_APPS_ABSTRACTCONTENTTYPE_H
#define LIGI_APPS_ABSTRACTCONTENTTYPE_H

#include <string>

class AbstractContentType {
public:
    AbstractContentType() = default;
    AbstractContentType(const AbstractContentType&) = delete;

    AbstractContentType& operator=(AbstractContentType const&) = delete;
    AbstractContentType(AbstractContentType&&) = delete;
    AbstractContentType& operator=(AbstractContentType&&) = delete;
    virtual ~AbstractContentType() = default;

    [[nodiscard]] virtual std::string get() const = 0;
    [[nodiscard]] virtual std::string fieldname() const final { return "Content-Type: "; }

    virtual explicit operator std::string()
    {
        return get();
    }
};
#endif //LIGI_APPS_ABSTRACTCONTENTTYPE_H
