#ifndef UTILITY_H
#define UTILITY_H

#include <iomanip>
#include <qdatetime.h>
#include <string>
#include <vector>

#include "Times.h"

class Utility
{
public:
    static std::vector<std::string> split(const std::string &str, char delimiter);

    static std::string formatNumber(std::size_t num);

    static Date convertQTDateToDate(const QDate& date);

    static QDateTime chronoToQDateTime(const std::chrono::year_month_day& date,
                                       const std::chrono::hh_mm_ss<std::chrono::seconds>& time);
};

#endif //UTILITY_H