/**
 * @file Habit.h
 * @brief 习惯记录类，用于管理用户的习惯信息
 * @author 冰柠
 * @date 2025年06月18日
 */

#ifndef HABIT_H
#define HABIT_H
#include "Times.h"

/**
 * @struct Habit
 * @brief 表示用户的习惯记录，包含习惯的各种属性和状态
 */
struct Habit
{
    std::size_t habit_id;       /**< 习惯唯一标识ID */
    std::size_t user_id;        /**< 关联的用户ID */
    std::string name;           /**< 习惯名称 */
    std::size_t target_count;   /**< 目标完成次数 */
    Date start_date;            /**< 习惯开始日期 */
    Date end_date;              /**< 习惯结束日期 */
    bool is_active;             /**< 是否活跃标志 */
    bool is_deleted;            /**< 是否已删除标志 */

    /**
     * @brief 构造函数，初始化习惯对象
     * @param habit_id 习惯ID，默认为0
     * @param user_id 用户ID，默认为0
     * @param name 习惯名称，默认为空字符串
     * @param target_count 目标次数，默认为0
     * @param start_date 开始日期，默认为空
     * @param end_date 结束日期，默认为空
     * @param is_active 是否活跃，默认为false
     * @param is_deleted 是否删除，默认为false
     */
    explicit Habit(
            std::size_t habit_id = 0,
            std::size_t user_id = 0,
            std::string name = "",
            std::size_t target_count = 0,
            Date start_date = {},
            Date end_date = {},
            bool is_active = false,
            bool is_deleted = false
    );
};

#endif //HABIT_H