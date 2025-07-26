/**
 * @file ServiceLayer.h
 * @brief 服务层头文件，封装业务逻辑，协调视图层与数据层交互
 * @author 遥远 冰柠 Rain
 */
#ifndef SERVICELAYER_H
#define SERVICELAYER_H

#include <queue>
#include "DateRecord.h"
#include "DBLayer.h"
#include "Pomodoro.h"

/**
 * @class ServiceLayer
 * @brief 服务层类，实现业务逻辑处理，作为视图层与数据层的中间层
 * @author 遥远 冰柠 Rain
 */
class ServiceLayer
{
private:
    DBLayer db_layer; /**< 数据层对象，用于数据库操作 */

public:
    /**
     * @brief 插入新习惯到系统
     * @author 冰柠
     * @param name 习惯名称
     * @param start_date 习惯开始日期
     * @param end_date 习惯结束日期
     * @param times_per_day 每日打卡次数
     * @return 插入是否成功，成功返回true，失败返回false
     * @details 验证习惯创建界面的用户输入数据是否合法，如日期区间、打卡次数等。
     *          不合法则返回false，合法将数据封装为Habit对象并传递给数据层。
     */
    bool insertHabit(const std::string& name, const Date& start_date, const Date& end_date, std::size_t times_per_day);

    /**
     * @brief 更新指定ID的习惯信息
     * @author 冰柠
     * @param habit_id 要修改的习惯ID
     * @param start_date 修改后的开始日期
     * @param end_date 修改后的结束日期
     * @param times_per_day 修改后的每日打卡次数
     * @param active_flag 是否启用该习惯
     * @return 更新是否成功，成功返回true，失败返回false
     * @details 验证习惯修改界面的用户输入数据是否合法，合法则更新数据层中的习惯记录。
     */
    bool updateHabit(std::size_t habit_id, const Date& start_date, const Date& end_date, std::size_t times_per_day, bool active_flag);

    /**
     * @brief 软删除指定ID的习惯
     * @author 冰柠
     * @param habit_id 要删除的习惯ID
     * @return 删除是否成功，成功返回true，失败返回false
     * @details 调用数据层的删除函数，执行软删除（修改删除标记字段，不真正删除数据）。
     */
    bool deleteHabit(std::size_t habit_id);

    /**
     * @brief 插入新事项到系统
     * @author 冰柠
     * @param name 事项名称
     * @param event_date 事项日期
     * @param event_time 事项时间
     * @param remind_flag 是否开启提醒
     * @param remind_time 提前提醒的时间
     * @return 插入是否成功，成功返回true，失败返回false
     * @details 验证事项创建界面的用户输入数据是否合法，合法则封装为Event对象并传递给数据层。
     */
    bool insertEvent(const std::string& name, const Date& event_date, const Time& event_time, bool remind_flag, const Time& remind_time);

    /**
     * @brief 更新指定ID的事项信息
     * @author 冰柠
     * @param event_id 要修改的事项ID
     * @param title 修改后的事项标题
     * @param event_date 修改后的事项日期
     * @param event_time 修改后的事项时间
     * @param remind_flag 是否开启提醒
     * @param remind_time 修改后的提醒时间
     * @return 更新是否成功，成功返回true，失败返回false
     * @details 验证事项修改界面的用户输入数据是否合法，合法则更新数据层中的事项记录。
     */
    bool updateEvent(std::size_t event_id, std::string title, const Date& event_date, const Time& event_time, bool remind_flag, const Time& remind_time);

    /**
     * @brief 软删除指定ID的事项
     * @author 冰柠
     * @param event_id 要删除的事项ID
     * @return 删除是否成功，成功返回true，失败返回false
     * @details 调用数据层的删除函数，执行软删除（修改删除标记字段，不真正删除数据）。
     */
    bool deleteEvent(int event_id);

    /**
     * @brief 记录习惯打卡
     * @author 冰柠
     * @param habit 打卡的习惯对象
     * @return 打卡是否成功，成功返回true，失败返回false
     * @details 习惯打卡，调用数据层函数插入到每日记录中。
     */
    bool checkinHabit(const Habit& habit);

    /**
     * @brief 获取所有活跃习惯
     * @author 冰柠
     * @return 活跃习惯列表
     * @details 通过数据层获取习惯列表，筛选出活跃字段为true的习惯。
     */
    [[nodiscard]] std::vector<Habit> getActiveHabits() const;

    /**
     * @brief 获取所有非活跃习惯
     * @author 冰柠
     * @return 非活跃习惯列表
     * @details 通过数据层获取习惯列表，筛选出活跃字段为false或已删除的习惯。
     */
    [[nodiscard]] std::vector<Habit> getInactiveHabits() const;

    /**
     * @brief 获取所有活跃事项
     * @author 冰柠
     * @return 活跃事项列表
     * @details 通过数据层获取事项列表，筛选出未过期且未删除的事项。
     */
    [[nodiscard]] std::vector<Event> getActiveEvents() const;

    /**
     * @brief 获取所有过期事项
     * @author 冰柠
     * @return 过期事项列表
     * @details 通过数据层获取事项列表，筛选出已过期或已删除的事项。
     */
    [[nodiscard]] std::vector<Event> getExpiredEvents() const;

    /**
     * @brief 番茄钟计时处理
     * @author Rain
     * @param pomodoro 番茄钟结构体
     * @param count_time 计时时间
     * @return 计时状态，未到时返回true，到达时间返回false
     * @details 判断番茄钟是否到时，通过传入的Pomodoro对象中的开始时间和倒计时时间相加得到结束时间，
     *          时间未到前返回true，到达后返回false，并把对应的番茄钟记录插入数据库。
     */
    bool pomodoroTick(const Pomodoro& pomodoro, const Time& count_time);

    /**
     * @brief 获取指定月份的习惯打卡记录统计
     * @author Rain
     * @param date 指定日期（使用其年月部分）
     * @return 每月每天的实际打卡数和应当打卡数
     * @details 获取某个月每一天打卡的习惯次数统计，调用数据层接口获取记录并计算统计值。
     */
    [[nodiscard]] std::vector<std::pair<std::size_t, std::size_t>> getHabitRecordsByDate(const Date& date) const;

    /**
     * @brief 获取指定日期的所有记录
     * @author Rain
     * @param date 指定日期
     * @return 对应日期的所有记录
     * @details 调用数据层接口，获取指定日期的习惯打卡和番茄钟使用记录。
     */
    [[nodiscard]] DateRecord getAllRecordsByDate(const Date& date);

    /**
     * @brief 获取当前时间戳（日期和时间）
     * @author 遥远
     * @return 当前日期和时间
     * @details 获取系统当前时间，封装为Date和Time对象返回，用于时间相关操作的时间基准。
     */
    [[nodiscard]] std::pair<Date, Time> getCurrentTimeStamp() const;

    /**
     * @brief 按ID获取习惯
     * @author 遥远
     * @param habit_id 习惯ID
     * @return 指定ID的习惯对象
     * @details 在习惯列表中查找指定ID的习惯，找到后返回，未找到时可能返回默认对象。
     */
    [[nodiscard]] Habit getHabitByID(std::size_t habit_id) const;

    /**
     * @brief 按ID获取事项
     * @author 遥远
     * @param event_id 事项ID
     * @return 指定ID的事项对象
     * @details 在事项列表中查找指定ID的事项，找到后返回，未找到时可能返回默认对象。
     */
    [[nodiscard]] Event getEventByID(std::size_t event_id) const;

    std::vector<Habit> getHabitsByDate(QDate date) const;

    std::vector<Event> getEventsByDate(QDate date) const;

    /**
     * @brief 初始化服务层
     * @author Rain
     * @details 初始化数据层，创建必要的数据库表结构，确保系统正常运行。
     */
    void init();

    /**
     * @brief 保存番茄钟状态到数据库
     * @author Rain
     * @param state 番茄钟状态 (0=IDLE, 1=RUNNING, 2=PAUSED)
     * @param total_seconds 总秒数
     * @param remaining_seconds 剩余秒数
     * @param remark 备注
     * @param start_time 开始时间戳
     * @return 保存是否成功
     */
    bool savePomodoroState(int state, int total_seconds, int remaining_seconds, const std::string& remark, const std::string& start_time);

    /**
     * @brief 从数据库加载番茄钟状态
     * @author Rain
     * @param state 输出：番茄钟状态
     * @param total_seconds 输出：总秒数
     * @param remaining_seconds 输出：剩余秒数
     * @param remark 输出：备注
     * @param start_time 输出：开始时间戳
     * @return 加载是否成功
     */
    bool loadPomodoroState(int& state, int& total_seconds, int& remaining_seconds, std::string& remark, std::string& start_time);

    /**
     * @brief 清除番茄钟状态
     * @author Rain
     * @return 清除是否成功
     */
    bool clearPomodoroState();
};

#endif // SERVICELAYER_H