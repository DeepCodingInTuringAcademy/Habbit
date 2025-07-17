/**
 * @file Event.h
 * @brief 事项记录类，用于管理用户的事项信息
 * @author 冰柠
 * @date 2025年06月18日
 */

#ifndef EVENT_H
#define EVENT_H
#include "Times.h"

/**
 * @struct Event
 * @brief 表示用户的事项记录，包含事项的各种属性和状态
 */
struct Event
{
    std::size_t event_id;       /**< 事项唯一标识ID */
    std::size_t user_id;        /**< 关联的用户ID */
    std::string title;          /**< 事项标题 */
    Date event_date;            /**< 事项日期 */
    Time event_time;            /**< 事项时间 */
    bool remind_flag;           /**< 是否设置提醒标志 */
    Time remind_time;           /**< 提醒时间 */
    bool is_expired_flag;       /**< 是否已过期标志 */
    bool is_deleted;            /**< 是否已删除标志 */

    /**
     * @brief 构造函数，初始化事项对象
     * @param event_id 事项ID，默认为0
     * @param user_id 用户ID，默认为0
     * @param title 事项标题，默认为空字符串
     * @param event_date 事项日期，默认为空
     * @param event_time 事项时间，默认为空
     * @param remind_flag 是否提醒，默认为false
     * @param remind_time 提醒时间，默认为空
     * @param is_expired_flag 是否过期，默认为false
     * @param is_deleted 是否删除，默认为false
     */
    explicit Event(
            std::size_t event_id = 0, //NOLINT
            std::size_t user_id = 0,
            std::string title = "",
            Date event_date = {},
            Time event_time = {},
            bool remind_flag = false,
            Time remind_time = {},
            bool is_expired_flag = false,
            bool is_deleted = false);
};

#endif //EVENT_H