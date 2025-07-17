/**
 * @file Pomodoro.h
 * @brief 番茄钟记录类，用于管理用户的番茄钟信息
 * @author 冰柠
 * @date 2025年06月18日
 */

#ifndef POMODORO_H
#define POMODORO_H
#include "Times.h"

/**
 * @struct Pomodoro
 * @brief 表示用户的番茄钟记录，包含番茄钟时间和记录信息
 */
struct Pomodoro
{
    std::size_t id;             /**< 番茄钟记录唯一标识ID */
    Time pomodoro_time;         /**< 番茄钟持续时间 */
    std::string record;         /**< 番茄钟记录备注 */

    /**
     * @brief 构造函数，初始化番茄钟对象
     * @param id 番茄钟ID
     * @param pomodoro_time 番茄钟持续时间
     * @param record 番茄钟记录备注
     */
    explicit Pomodoro(
            std::size_t id,
            Time pomodoro_time,
            std::string record
    );
};

#endif //POMODORO_H