#ifndef TIMES_H
#define TIMES_H
#include <chrono>

using Date = std::chrono::year_month_day;

using Time = std::chrono::hh_mm_ss<std::chrono::seconds>;

std::string toString(const Date& date);

std::string toString(const Time& time);

#endif //TIMES_H
