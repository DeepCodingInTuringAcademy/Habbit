#include "Times.h"

std::string toString(const Date& date)
{
    char buffer[11]; // 足够存放 "YYYY-MM-DD\0"
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
             static_cast<int>(date.year()),
             static_cast<unsigned>(date.month()),
             static_cast<unsigned>(date.day()));
    return std::string(buffer);
}

std::string toString(const Time& time)
{
    char buffer[9]; // 足够存放 "HH:MM:SS\0"
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d",
             static_cast<int>(time.hours().count()),
             static_cast<int>(time.minutes().count()),
             static_cast<int>(time.seconds().count()));
    return std::string(buffer);
}

// 字符串转日期 (格式: YYYY-MM-DD)
Date dateFromString(const std::string& date_str)
{
    std::istringstream iss(date_str);
    int year, month, day;
    char delim1, delim2;

    if (!(iss >> year >> delim1 >> month >> delim2 >> day) ||
        delim1 != '-' || delim2 != '-')
    {
        throw std::invalid_argument("Invalid date format. Expected YYYY-MM-DD");
    }

    return Date{
            std::chrono::year{year},
            std::chrono::month{static_cast<unsigned>(month)},
            std::chrono::day{static_cast<unsigned>(day)}
    };
}

// 字符串转时间 (格式: HH:MM:SS)
Time timeFromString(const std::string& time_str)
{
    std::istringstream iss(time_str);
    int hours, minutes, seconds;
    char delim1, delim2;

    if (!(iss >> hours >> delim1 >> minutes >> delim2 >> seconds) ||
        delim1 != ':' ||
        delim2 != ':')
    {
        throw std::invalid_argument("Invalid time format. Expected HH:MM:SS");
    }

    return Time
    {
            std::chrono::hours{hours} +
            std::chrono::minutes{minutes} +
            std::chrono::seconds{seconds}
    };
}

std::strong_ordering operator<=>(const Time &lhs, const Time &rhs)
{
    return lhs.to_duration() <=> rhs.to_duration();
}

Time operator+(const Time& lhs, const Time& rhs)
{
    auto lhs_duration = std::chrono::seconds(lhs.to_duration().count());
    auto rhs_duration = std::chrono::seconds(rhs.to_duration().count());

    auto total_duration = lhs_duration + rhs_duration;

    return Time(total_duration);
}
