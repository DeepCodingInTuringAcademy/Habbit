/**
 * @file Utility.h
 * @brief 工具函数集合
 * @author 冰柠
 * @date 2025年06月18日
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <iomanip>
#include <qdatetime.h>
#include <string>
#include <vector>

#include "Times.h"

/**
 * @class Utility
 * @brief 提供各种实用工具函数
 */
class Utility
{
public:
    /**
     * @brief 分割字符串
     * @param str 要分割的字符串
     * @param delimiter 分隔符
     * @return 分割后的字符串向量
     */
    static std::vector<std::string> split(const std::string &str, char delimiter);

    /**
     * @brief 格式化数字为字符串
     * @param num 要格式化的数字
     * @return 格式化后的字符串
     */
    static std::string formatNumber(std::size_t num);

    /**
     * @brief 将QDate转换为Date类型
     * @param date QDate对象
     * @return 转换后的Date对象
     */
    static Date convertQTDateToDate(const QDate& date);

    /**
     * @brief 将chrono时间类型转换为QDateTime
     * @param date chrono日期对象
     * @param time chrono时间对象
     * @return 转换后的QDateTime对象
     */
    static QDateTime chronoToQDateTime(const std::chrono::year_month_day& date,
                                       const std::chrono::hh_mm_ss<std::chrono::seconds>& time);

    [[nodiscard]] std::pair<Date, Time> getCurrentTimeStamp() const;
};

#endif //UTILITY_H