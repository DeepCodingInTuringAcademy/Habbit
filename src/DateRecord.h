/**
 * @file DateRecord.h
 * @brief 每日记录类，用于管理某一天的习惯和番茄钟记录
 * @author 冰柠
 * @date 2025年06月18日
 */

#ifndef DATERECORD_H
#define DATERECORD_H

#include <vector>

#include "Habit.h"
#include "Pomodoro.h"
#include "Event.h"

/**
 * @struct DateRecord
 * @brief 表示某一天的时间记录，包含习惯完成情况和番茄钟使用记录
 *
 * 该类用于每日时间线显示功能，按时间顺序记录一天中完成的习惯和使用的番茄钟，
 * 便于用户在时间线视图中查看每日活动情况。
 */
struct DateRecord
{
    std::vector<std::pair<Time, Habit>> habit_records;       /**< 习惯记录向量，每个元素为<完成时间, 习惯对象>对 */
    std::vector<std::pair<Time, Pomodoro>> pomodoro_records; /**< 番茄钟记录向量，每个元素为<使用时间, 番茄钟对象>对 */
    std::vector<std::pair<Time, Event>> event_records;       /**< 事件记录向量，每个元素为<发生时间, 事件对象>对 */

    /**
     * @brief 构造函数，初始化每日记录对象
     * @param habit_records 习惯记录向量，默认为空
     * @param pomodoro_records 番茄钟记录向量，默认为空
     * @param event_records 事件记录向量，默认为空
     */
    explicit DateRecord(
        const std::vector<std::pair<Time, Habit>> &habit_records = {},
        const std::vector<std::pair<Time, Pomodoro>> &pomodoro_records = {},
        const std::vector<std::pair<Time, Event>> &event_records = {});

    /**
     * @brief 获取当日记录总数（习惯记录数 + 番茄钟记录数）
     * @return 当日记录的总数量
     * @note 使用[[nodiscard]]确保返回值不被忽略
     */
    [[nodiscard]] std::size_t getSize() const;
};

#endif // DATERECORD_H