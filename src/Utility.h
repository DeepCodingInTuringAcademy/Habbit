#ifndef UTILITY_H
#define UTILITY_H

#include <iomanip>
#include <string>
#include <vector>

class Utility
{
public:
    static std::vector<std::string> split(const std::string &str, char delimiter);

    static std::string formatNumber(std::size_t num);
};

#endif //UTILITY_H