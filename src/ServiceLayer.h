/**
 * @file ServiceLayer.h
 * @brief 服务层头文件，封装业务逻辑，协调视图层与数据层交互
 * @author 遥远 冰柠 Rain
 */
#ifndef SERVICELAYER_H
#define SERVICELAYER_H

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
    static const QString THEMES_PATH; // 主题列表配置文件路径
    static const QString CURRENT_THEME_PATH; // 当前主题配置文件路径

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
     * @param new_name
     * @param start_date 修改后的开始日期
     * @param end_date 修改后的结束日期
     * @param times_per_day 修改后的每日打卡次数
     * @param active_flag 是否启用该习惯
     * @return 更新是否成功，成功返回true，失败返回false
     * @details 验证习惯修改界面的用户输入数据是否合法，合法则更新数据层中的习惯记录。
     */
    bool updateHabit(std::size_t habit_id, std::string new_name, const Date& start_date, const Date& end_date, std::size_t times_per_day, bool active_flag);

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
     * @brief 设置启用习惯
     * @author Rain
     * @return 是否启用成功
     */
    bool inactiveHabit(std::size_t habit_id);

    /**
     * @brief 设置停用习惯
     * @author Rain
     * @return 是否停用成功
     */
    bool activeHabit(std::size_t habit_id);

    /**
     * @brief 获取所有过期事项
     * @author 冰柠
     * @return 过期事项列表
     * @details 通过数据层获取事项列表，筛选出已过期或已删除的事项。
     */
    [[nodiscard]] std::vector<Event> getExpiredEvents() const;

    /**
     * @brief 将番茄钟记录插入到数据库中
     * @author Rain
     * @param pomodoro 番茄钟结构体
     * @return 插入状态，插入成功返回true，插入失败返回false
     */
    bool insertPomoRecord(const Pomodoro& pomodoro);

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
    [[nodiscard]] DateRecord getAllRecordsByDate(const Date& date) const;

    /**
     * @brief 获取当前时间戳（日期和时间）
     * @author 遥远
     * @return 当前日期和时间
     * @details 获取系统当前时间，封装为Date和Time对象返回，用于时间相关操作的时间基准。
     */
    [[nodiscard]] static std::pair<Date, Time> getCurrentTimeStamp();

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

    std::size_t getHabitCheckInCount(const Habit& habit) const;

    /**
     * @brief 初始化服务层
     * @author Rain
     * @details 初始化数据层，创建必要的数据库表结构，确保系统正常运行。
     */
    void init();

    /**
     * @brief 获取所有可用主题列表
     * @return 主题名称列表
     */
    static QStringList getAvailableThemes();

    /**
     * @brief 获取当前使用的主题名称
     * @return 当前主题名称
     */
    static QString getCurrentThemeName();

    /**
     * @brief 设置当前主题
     * @param theme_name 主题名称
     * @return 是否设置成功
     */
    static bool setCurrentTheme(const QString &theme_name);

    /**
     * @brief 获取主题配置
     * @param theme_name 主题名称
     * @return 主题配置的JSON对象
     */
    static QJsonObject getThemeConfig(const QString &theme_name);

    bool savePomodoroState(int state, int total_seconds, int remaining_seconds, const std::string &remark,
                           const std::string &start_time) const;

    bool loadPomodoroState(int &state, int &total_seconds, int &remaining_seconds, std::string &remark,
                           std::string &start_time) const;

    bool clearPomodoroState() const;
};

inline const QString ServiceLayer::THEMES_PATH = ":/themes/themes.json"; // 主题列表配置文件路径
inline const QString ServiceLayer::CURRENT_THEME_PATH = ":/config/current_theme.json"; // 当前主题配置文件路径

#endif // SERVICELAYER_H