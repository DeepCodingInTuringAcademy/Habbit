#include "Utility.h"

std::vector<std::string> Utility::split(const std::string& str, const char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

std::string Utility::formatNumber(const std::size_t num)
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << num;
    return oss.str();
}

Date Utility::convertQTDateToDate(const QDate &QTdate)
{
    // TODO
    // int year = QTdate.year();
    // unsigned month = static_cast<unsigned>(QTdate.month());
    // unsigned day = static_cast<unsigned>(QTdate.day());
    // Date my_date{std::chrono::year{year}, std::chrono::month{month}, std::chrono::day{day}};
    // return my_date;
}

QDateTime chronoToQDateTime(const std::chrono::year_month_day& date, const std::chrono::hh_mm_ss<std::chrono::seconds>& time) {
    const int year = static_cast<int>(date.year());
    const unsigned int month = static_cast<unsigned>(date.month());
    const unsigned int day = static_cast<unsigned>(date.day());

    const int hour = time.hours().count();
    const int minute = time.minutes().count();
    const int second = time.seconds().count();

    return QDateTime(QDate(year, month, day), QTime(hour, minute, second));
}