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

// 字符串转日期 (格式: YYYY-MM-DD)
Date dateFromString(const std::string& date_str) {
    std::istringstream iss(date_str);
    int year, month, day;
    char delim1, delim2;

    if (!(iss >> year >> delim1 >> month >> delim2 >> day) ||
        delim1 != '-' || delim2 != '-') {
        throw std::invalid_argument("Invalid date format. Expected YYYY-MM-DD");
    }

    return Date{
            std::chrono::year{year},
            std::chrono::month{static_cast<unsigned>(month)},
            std::chrono::day{static_cast<unsigned>(day)}
    };
}

// 字符串转时间 (格式: HH:MM:SS)
Time timeFromString(const std::string& time_str) {
    std::istringstream iss(time_str);
    int hours, minutes, seconds;
    char delim1, delim2;

    if (!(iss >> hours >> delim1 >> minutes >> delim2 >> seconds) ||
        delim1 != ':' || delim2 != ':') {
        throw std::invalid_argument("Invalid time format. Expected HH:MM:SS");
    }

    return Time{
            std::chrono::hours{hours} +
            std::chrono::minutes{minutes} +
            std::chrono::seconds{seconds}
    };
}

std::strong_ordering operator<=>(const Time &lhs, const Time &rhs) {
    return lhs.to_duration() <=> rhs.to_duration();
}
