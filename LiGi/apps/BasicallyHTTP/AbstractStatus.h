//
// Created by darksider3 on 22.12.20.
//

#ifndef LIGI_APPS_ABSTRACTSTATUS_H
#define LIGI_APPS_ABSTRACTSTATUS_H

#include <string>
/**
 * @brief The AbstractStatus class provides an base class for status codes, like the `Status404` class.
 */
class AbstractStatus {
private:
protected:
public:
    /**
     * @brief get returns the appropriate status code as a string
     * @return Statuscode as std::string
     */
    [[nodiscard]] virtual std::string get() const = 0;
    /**
	 * @brief operator std::string std::string conversion operator
	 * @return Same as `.get()`.
	 */
    virtual explicit operator std::string()
    {
        return get();
    }
    AbstractStatus() = default;
    AbstractStatus(const AbstractStatus&) = delete;
    AbstractStatus& operator=(AbstractStatus const&) = delete;
    AbstractStatus(AbstractStatus&&) = delete;
    AbstractStatus& operator=(AbstractStatus&&) = delete;
    virtual ~AbstractStatus() = default;
};
#endif //LIGI_APPS_ABSTRACTSTATUS_H
