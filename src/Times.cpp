#include "Times.h"

std::string toString(const Date& date)
{
    return std::to_string(static_cast<int>(date.year())) + "-"
         + std::to_string(static_cast<unsigned int>(date.month())) + "-"
         + std::to_string(static_cast<unsigned int>(date.day()));
}

std::string toString(const Time& time)
{
    return std::to_string(time.hours().count()) + ":"
         + std::to_string(time.minutes().count()) + ":"
         + std::to_string(time.seconds().count());
}
