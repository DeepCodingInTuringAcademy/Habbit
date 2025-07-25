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

Date Utility::convertQTDateToDate(const QDate& date)
{
    const auto year = date.year();
    const auto month = static_cast<unsigned>(date.month());
    const auto day = static_cast<unsigned>(date.day());

    return {std::chrono::year{year}, std::chrono::month{month}, std::chrono::day{day}};
}

QDateTime Utility::chronoToQDateTime(const Date& date, const Time& time)
{
    const auto year = static_cast<int>(date.year());
    const auto month = static_cast<int>(static_cast<unsigned>(date.month()));
    const auto day = static_cast<int>(static_cast<unsigned>(date.day()));

    const auto hour = static_cast<int>(time.hours().count());
    const auto minute = static_cast<int>(time.minutes().count());
    const auto second = static_cast<int>(time.seconds().count());

    return {{year, month, day}, {hour, minute, second}};
}


std::pair<Date, Time> Utility::getCurrentTimeStamp()
{
    auto now_time = std::chrono::system_clock::now();
    auto local_time = std::chrono::current_zone()->to_local(now_time);

    auto time_since_midnight = local_time - std::chrono::floor<std::chrono::days>(local_time);
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_midnight);

    return
 {
     std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(local_time)),
     std::chrono::hh_mm_ss<std::chrono::seconds>(seconds)
 };
}
