#ifndef TIMES_H
#define TIMES_H
#include <chrono>

using Date = std::chrono::year_month_day;

using Time = std::chrono::hh_mm_ss<std::chrono::seconds>;

std::string toString(const Date& date);

std::string toString(const Time& time);

Date dateFromString(const std::string& date_str);  // 格式: "YYYY-MM-DD"

Time timeFromString(const std::string& time_str);  // 格式: "HH:MM:SS"

std::strong_ordering operator<=>(const Time& lhs, const Time& rhs);

Time operator+(const Time& lhs, const Time& rhs);

#endif //TIMES_H
