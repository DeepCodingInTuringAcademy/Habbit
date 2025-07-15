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
#include "Event.h"
#include "Habit.h"

/**
 * @class DBLayer
 * @brief 数据层类，负责与数据库交互，实现数据的增删改查操作
 * @author XTUG
 */
class DBLayer
{
private:
    std::string db_file_name_; /**< 数据库文件名称 */
    QSqlDatabase db_;          /**< Qt数据库对象 */

    /**
     * @fn bool openDatabase()
     * @brief 打开数据库连接
     * @author XTUG
     * @return bool 打开是否成功，成功返回true，失败返回false
     * @details 尝试建立与数据库的连接，使用Qt的QSqlDatabase实现
     */
    [[nodiscard]] bool openDatabase();

    /**
     * @fn void closeDatabase()
     * @brief 关闭数据库连接
     * @author XTUG
     * @details 关闭已打开的数据库连接，释放资源
     */
    void closeDatabase();

public:
    /**
     * @fn DBLayer(std::string db_file_name = "")
     * @brief 构造函数，初始化数据层对象
     * @author XTUG
     * @param db_file_name 数据库文件名称，默认为空字符串
     * @details 初始化数据库文件路径，并尝试连接数据库
     */
    explicit DBLayer(std::string db_file_name = "");

    /**
     * @fn ~DBLayer()
     * @brief 析构函数，释放资源
     * @author XTUG
     * @details 关闭数据库连接，释放资源
     */
    ~DBLayer();

    /**
     * @fn std::vector<Habit> getHabitLists()
     * @brief 获取所有习惯列表
     * @author XTUG
     * @return std::vector<Habit> 习惯对象列表
     * @details 从数据库中查询所有习惯记录，转换为Habit对象列表返回
     */
    [[nodiscard]] std::vector<Habit> getHabitLists();

    /**
     * @fn bool insertHabit(const Habit& habit)
     * @brief 插入新习惯到数据库
     * @author XTUG
     * @param habit 要插入的习惯对象
     * @return bool 插入是否成功，成功返回true，失败返回false
     * @details 将习惯对象的数据插入到数据库的习惯表中
     */
    bool insertHabit(const Habit &habit);

    /**
     * @fn bool updateHabit(const Habit& habit)
     * @brief 更新指定ID的习惯
     * @author XTUG
     * @param habit 要更新的习惯对象
     * @return bool 更新是否成功，成功返回true，失败返回false
     * @details 根据习惯ID更新数据库中对应的习惯记录
     */
    bool updateHabit(const Habit &habit);

    /**
     * @fn bool deleteHabit(std::size_t habit_id)
     * @brief 删除指定ID的习惯
     * @author XTUG
     * @param habit_id 要删除的习惯ID
     * @return bool 删除是否成功，成功返回true，失败返回false
     * @details 从数据库中删除指定ID的习惯记录
     */
    bool deleteHabit(std::size_t habit_id);

    /**
     * @fn std::vector<Event> getEventLists()
     * @brief 获取所有事项列表
     * @author XTUG
     * @return std::vector<Event> 事项对象列表
     * @details 从数据库中查询所有事项记录，转换为Event对象列表返回
     */
    [[nodiscard]] std::vector<Event> getEventLists();

    /**
     * @fn bool insertEvent(const Event& event)
     * @brief 插入新事项到数据库
     * @author XTUG
     * @param event 要插入的事项对象
     * @return bool 插入是否成功，成功返回true，失败返回false
     * @details 将事项对象的数据插入到数据库的事项表中
     */
    bool insertEvent(const Event &event);

    /**
     * @fn bool updateEvent(const Event& event)
     * @brief 更新指定ID的事项
     * @author XTUG
     * @param event 要更新的事项对象
     * @return bool 更新是否成功，成功返回true，失败返回false
     * @details 根据事项ID更新数据库中对应的事项记录
     */
    bool updateEvent(const Event &event);

    /**
     * @fn bool deleteEvent(std::size_t event_id)
     * @brief 删除指定ID的事项
     * @author XTUG
     * @param event_id 要删除的事项ID
     * @return bool 删除是否成功，成功返回true，失败返回false
     * @details 从数据库中删除指定ID的事项记录
     */
    bool deleteEvent(std::size_t event_id);
};

#endif // DBLAYER_H
