#include "utility.h"
#include <iomanip>


std::vector<std::string> utility::split(const std::string& str, const char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string utility::formatNumber(const std::size_t num)
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << num;
    return oss.str();
}
