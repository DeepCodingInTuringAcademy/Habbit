/**
 * @file DBLayer.h
 * @brief 数据层头文件，封装数据库操作，使用Qt的SQL库与数据库交互
 * @author XTUG
 * @date 2025年06月18日
 */

#ifndef DBLAYER_H
#define DBLAYER_H

#include <vector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include "DateRecord.h"
#include "Event.h"
#include "Habit.h"
#include "Pomodoro.h"

/**
 * @struct UserSettings
 * @brief 用户设置结构体，存储用户的各项设置
 * @author XTUG
 */
struct UserSettings
{
    std::size_t user_id;            /**< 用户ID */
    std::string nickname;           /**< 用户昵称 */
    std::string current_skin;       /**< 当前UI皮肤 */
    bool ddl_reminder_enabled;      /**< 是否启用DDL提醒 */
    std::string ddl_reminder_email; /**< DDL提醒邮箱 */
    std::string last_login_time;    /**< 上次登录时间 */
    bool is_logged_in;              /**< 是否已登录 */
};

/**
 * @class DBLayer
 * @brief 数据层类，负责与数据库交互，实现数据的增删改查操作
 * @author XTUG
 */
class DBLayer
{
private:
    std::string db_file_name_; /**< 数据库文件名称 */
    mutable QSqlDatabase db_;  /**< Qt数据库对象 */

    /**
     * @brief 打开数据库连接
     * @author XTUG
     * @return 打开是否成功，成功返回true，失败返回false
     * @details 尝试建立与数据库的连接，使用Qt的QSqlDatabase实现
     */
    [[nodiscard]] bool openDatabase() const;

    /**
     * @brief 关闭数据库连接
     * @author XTUG
     * @details 关闭已打开的数据库连接，释放资源
     */
    void closeDatabase() const;

public:
    /**
     * @brief 构造函数，初始化数据层对象
     * @author XTUG
     * @param db_file_name 数据库文件名称，默认为 HabbitDB.db
     * @details 初始化数据库文件路径，并尝试连接数据库。建立数据库，建立数据表：习惯表、事项表、番茄钟表等
     */
    explicit DBLayer(std::string db_file_name = "HabbitDB.db");

    /**
     * @brief 析构函数，释放资源
     * @author XTUG
     * @details 关闭数据库连接，释放资源
     */
    ~DBLayer();

    /**
     * @brief 获取所有习惯列表
     * @author XTUG
     * @return std::vector<Habit> 习惯对象列表
     * @details 从数据库中查询所有习惯记录，转换为Habit对象列表返回
     */
    [[nodiscard]] std::vector<Habit> getHabitLists() const;

    /**
     * @brief 插入新习惯到数据库
     * @author XTUG
     * @param habit 要插入的习惯对象
     * @return 插入是否成功，成功返回true，失败返回false
     * @details 将习惯对象的数据插入到数据库的习惯表中
     */
    bool insertHabit(const Habit &habit);

    /**
     * @brief 更新指定ID的习惯
     * @author XTUG
     * @param habit 要更新的习惯对象
     * @return 更新是否成功，成功返回true，失败返回false
     * @details 根据习惯ID更新数据库中对应的习惯记录
     */
    bool updateHabit(const Habit &habit);

    /**
     * @brief 删除指定ID的习惯
     * @author XTUG
     * @param habit_id 要删除的习惯ID
     * @return 删除是否成功，成功返回true，失败返回false
     * @details 从数据库中删除指定ID的习惯记录
     */
    bool deleteHabit(std::size_t habit_id);

    /**
     * @brief 停用指定ID的习惯
     * @author XTUG
     * @param habit_id 要停用的习惯ID
     * @return 停用是否成功，成功返回true，失败返回false
     * @details 从数据库中停用指定ID的习惯
     */
    bool setInactiveHabit(std::size_t habit_id);

    /**
     * @brief 启用指定ID的习惯
     * @author XTUG
     * @param habit_id 要启用的习惯ID
     * @return 启用是否成功，成功返回true，失败返回false
     * @details 从数据库中启用指定ID的习惯
     */
    bool setActiveHabit(std::size_t habit_id);

    /**
     * @brief 插入一条习惯记录至记录表
     * @author XTUG
     * @param habit 要插入的习惯记录的习惯对象
     * @return 插入是否成功，成功返回true，失败返回false
     * @details 向数据库的记录表中插入一条习惯打卡记录，需要获取系统时间，建立打卡时间与记录的映射关系。
     */
    bool insertHabitRecord(const Habit &habit);

    /**
     * @brief 获取所有事项列表
     * @author XTUG
     * @return 事项对象列表
     * @details 从数据库中查询所有事项记录，转换为Event对象列表返回
     */
    [[nodiscard]] std::vector<Event> getEventLists() const;

    /**
     * @brief 插入新事项到数据库
     * @author XTUG
     * @param event 要插入的事项对象
     * @return 插入是否成功，成功返回true，失败返回false
     * @details 将事项对象的数据插入到数据库的事项表中
     */
    bool insertEvent(const Event &event);

    /**
     * @brief 更新指定ID的事项
     * @author XTUG
     * @param event 要更新的事项对象
     * @return 更新是否成功，成功返回true，失败返回false
     * @details 根据事项ID更新数据库中对应的事项记录
     */
    bool updateEvent(const Event &event);

    /**
     * @brief 删除指定ID的事项
     * @author XTUG
     * @param event_id 要删除的事项ID
     * @return 删除是否成功，成功返回true，失败返回false
     * @details 从数据库中删除指定ID的事项记录
     */
    bool deleteEvent(std::size_t event_id);

    /**
     * @brief 插入一条番茄钟记录
     * @author XTUG
     * @param pomo 需要插入的番茄钟记录
     */
    void insertPomoRecord(Pomodoro pomo);

    /**
     * @brief 根据日期获取记录
     * @author Darling
     * @param date
     * @return 指定日期的所有记录（包括习惯打卡记录、事项记录、番茄钟专注记录）
     */
    DateRecord getRecordbyDate(Date date) const;

    /**
     * @brief 获取数据库中当前最大的 Habit ID
     * @author XTUG
     * @return 数据库中的最大 Habit ID
     */
    int getHabitIDMax();

    /**
     * @brief 获取数据库中当前最大的 Event ID
     * @author 遥远
     * @return 数据库中的最大 Event ID
     */
    int getEventIDMax();

    /**
     * @brief 获取数据库中当前最大的 Pomo ID
     * @author 遥远
     * @return 数据库中的最大 Pomo ID
     */
    int getPomoIDMax();

    /**
     * @brief 获取用户设置
     * @author XTUG
     * @param user_id 用户ID
     * @return 用户设置对象
     * @details 从数据库中查询指定用户ID的设置信息
     */
    [[nodiscard]] UserSettings getUserSettings(std::size_t user_id) const;

    /**
     * @brief 更新用户设置
     * @author XTUG
     * @param settings 用户设置对象
     * @return 更新是否成功，成功返回true，失败返回false
     * @details 将用户设置更新到数据库中，如果用户不存在则创建新记录
     */
    bool updateUserSettings(const UserSettings &settings);

    /**
     * @brief 获取当前登录的用户ID
     * @author XTUG
     * @return 当前登录的用户ID，如果没有用户登录则返回0
     * @details 从数据库中查询当前登录状态为true的用户ID
     */
    [[nodiscard]] std::size_t getCurrentUserID() const;

    /**
     * @brief 停用指定ID的习惯
     * @author XTUG
     * @param habit_id 要停用的习惯ID
     * @return 停用是否成功，成功返回true，失败返回false
     * @details 从数据库中停用指定ID的习惯
     */
    bool setInactiveHabit(std::size_t habit_id);

    /**
     * @brief 启用指定ID的习惯
     * @author XTUG
     * @param habit_id 要启用的习惯ID
     * @return 启用是否成功，成功返回true，失败返回false
     * @details 从数据库中启用指定ID的习惯
     */
    bool setActiveHabit(std::size_t habit_id);
};

#endif // DBLAYER_H
