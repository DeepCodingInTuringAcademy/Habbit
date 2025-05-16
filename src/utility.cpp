#include "utility.h"
#include <iomanip>


std::string utility::formatNumber(const std::size_t num)
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << num;
    return oss.str();
}
