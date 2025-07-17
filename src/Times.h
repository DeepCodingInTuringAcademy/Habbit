/**
 * @file Times.h
 * @brief 时间相关类型和工具函数定义
 * @author 冰柠
 * @date 2025年06月18日
 */

#ifndef TIMES_H
#define TIMES_H
#include <chrono>

using Date = std::chrono::year_month_day; /**< 日期类型，年-月-日 */

using Time = std::chrono::hh_mm_ss<std::chrono::seconds>; /**< 时间类型，时:分:秒 */

/**
 * @brief 将Date对象转换为字符串
 * @param date 要转换的Date对象
 * @return 格式为"YYYY-MM-DD"的字符串
 */
std::string toString(const Date& date);

/**
 * @brief 将Time对象转换为字符串
 * @param time 要转换的Time对象
 * @return 格式为"HH:MM:SS"的字符串
 */
std::string toString(const Time& time);

/**
 * @brief 从字符串解析Date对象
 * @param date_str 格式为"YYYY-MM-DD"的字符串
 * @return 解析得到的Date对象
 */
Date dateFromString(const std::string& date_str);

/**
 * @brief 从字符串解析Time对象
 * @param time_str 格式为"HH:MM:SS"的字符串
 * @return 解析得到的Time对象
 */
Time timeFromString(const std::string& time_str);

/**
 * @brief 比较两个Time对象的时间先后
 * @param lhs 左侧Time对象
 * @param rhs 右侧Time对象
 * @return 比较结果
 */
std::strong_ordering operator<=>(const Time& lhs, const Time& rhs);

/**
 * @brief 两个Time对象相加
 * @param lhs 左侧Time对象
 * @param rhs 右侧Time对象
 * @return 相加后的Time对象
 */
Time operator+(const Time& lhs, const Time& rhs);

#endif //TIMES_H