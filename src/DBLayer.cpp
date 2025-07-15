#include "DBLayer.h"

DBLayer::DBLayer(std::string db_file_name) : db_file_name_(db_file_name)
{
    // 初始化 SQLite 数据库连接
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(QString::fromStdString(db_file_name_));

    // 检查数据库文件是否存在
    if (!QFile::exists(QString::fromStdString(db_file_name_)))
    {
        qDebug() << "数据库文件不存在，将创建新文件：" << QString::fromStdString(db_file_name_);
    }

    // 打开数据库
    if (!db_.open())
    {
        qDebug() << "数据库打开失败：" << db_.lastError().text();
        return;
    }

    // 初始化表
    QSqlQuery query(db_);
    // 创建 HabitTable
    QString habitSql = "CREATE TABLE IF NOT EXISTS HabitTable ("
                       "habitId INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "userId INTEGER, "
                       "name TEXT, "
                       "targetCount INTEGER, "
                       "startDate TEXT, "
                       "endDate TEXT, "
                       "isActive INTEGER, "
                       "isDeleted INTEGER)";
    if (!query.exec(habitSql))
    {
        qDebug() << "HabitTable 创建失败：" << query.lastError().text();
    }
    else
    {
        qDebug() << "HabitTable 创建成功";
    }

    // 创建 EventTable
    QString eventSql = "CREATE TABLE IF NOT EXISTS EventTable ("
                       "eventId INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "userId INTEGER, "
                       "title TEXT, "
                       "eventDate TEXT, "
                       "eventTime TEXT, "
                       "remindFlag INTEGER, "
                       "remindTime TEXT, "
                       "isExpiredFlag INTEGER, "
                       "isDeleted INTEGER)";
    if (!query.exec(eventSql))
    {
        qDebug() << "EventTable 创建失败：" << query.lastError().text();
    }
    else
    {
        qDebug() << "EventTable 创建成功";
    }

    // 构造函数中初始化完后关闭数据库
    closeDatabase();
}

DBLayer::~DBLayer()
{
    closeDatabase();
}

bool DBLayer::openDatabase()
{
    if (db_.isOpen())
    {
        return true;
    }
    return db_.open();
}

void DBLayer::closeDatabase()
{
    if (db_.isOpen())
    {
        db_.close();
    }
}

std::vector<Habit> DBLayer::getHabitLists()
{
    std::vector<Habit> habits;
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法获取习惯列表";
        return habits;
    }

    QSqlQuery query(db_);
    QString sql = "SELECT * FROM HabitTable WHERE isDeleted = 0";
    if (!query.exec(sql))
    {
        qDebug() << "查询习惯列表失败：" << query.lastError().text();
        closeDatabase();
        return habits;
    }

    while (query.next())
    {
        Habit habit(
            query.value("habitId").toInt(),
            query.value("userId").toInt(),
            query.value("name").toString(),
            query.value("targetCount").toInt(),
            Date(query.value("startDate").toString()),
            Date(query.value("endDate").toString()),
            query.value("isActive").toInt() == 1,
            query.value("isDeleted").toInt() == 1);
        habits.push_back(habit);
    }
    closeDatabase();
    return habits;
}

bool DBLayer::insertHabit(const Habit &habit)
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法插入习惯";
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("INSERT INTO HabitTable (userId, name, targetCount, startDate, endDate, isActive, isDeleted) "
                  "VALUES (:userId, :name, :targetCount, :startDate, :endDate, :isActive, :isDeleted)");
    query.bindValue(":userId", habit.userId);
    query.bindValue(":name", habit.name);
    query.bindValue(":targetCount", habit.targetCount);
    query.bindValue(":startDate", habit.startDate.toString());
    query.bindValue(":endDate", habit.endDate.toString());
    query.bindValue(":isActive", habit.isActive ? 1 : 0);
    query.bindValue(":isDeleted", habit.isDeleted ? 1 : 0);

    if (!query.exec())
    {
        qDebug() << "插入习惯失败：" << query.lastError().text();
        closeDatabase();
        return false;
    }
    closeDatabase();
    return true;
}

bool DBLayer::updateHabit(const Habit &habit)
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法更新习惯";
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("UPDATE HabitTable SET userId = :userId, name = :name, targetCount = :targetCount, "
                  "startDate = :startDate, endDate = :endDate, isActive = :isActive, isDeleted = :isDeleted "
                  "WHERE habitId = :habitId");
    query.bindValue(":habitId", habit.habitId);
    query.bindValue(":userId", habit.userId);
    query.bindValue(":name", habit.name);
    query.bindValue(":targetCount", habit.targetCount);
    query.bindValue(":startDate", habit.startDate.toString());
    query.bindValue(":endDate", habit.endDate.toString());
    query.bindValue(":isActive", habit.isActive ? 1 : 0);
    query.bindValue(":isDeleted", habit.isDeleted ? 1 : 0);

    if (!query.exec())
    {
        qDebug() << "更新习惯失败：" << query.lastError().text();
        closeDatabase();
        return false;
    }
    closeDatabase();
    return true;
}

bool DBLayer::deleteHabit(std::size_t habit_id)
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法删除习惯";
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("UPDATE HabitTable SET isDeleted = 1 WHERE habitId = :habitId");
    query.bindValue(":habitId", static_cast<int>(habit_id));
    if (!query.exec())
    {
        qDebug() << "删除习惯失败：" << query.lastError().text();
        closeDatabase();
        return false;
    }
    closeDatabase();
    return true;
}

std::vector<Event> DBLayer::getEventLists()
{
    std::vector<Event> events;
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法获取事项列表";
        return events;
    }

    QSqlQuery query(db_);
    QString sql = "SELECT * FROM EventTable WHERE isDeleted = 0";
    if (!query.exec(sql))
    {
        qDebug() << "查询事项列表失败：" << query.lastError().text();
        closeDatabase();
        return events;
    }

    while (query.next())
    {
        Event event(
            query.value("eventId").toInt(),
            query.value("userId").toInt(),
            query.value("title").toString(),
            Date(query.value("eventDate").toString()),
            Time(query.value("eventTime").toString()),
            query.value("remindFlag").toInt() == 1,
            Time(query.value("remindTime").toString()),
            query.value("isExpiredFlag").toInt() == 1,
            query.value("isDeleted").toInt() == 1);
        events.push_back(event);
    }
    closeDatabase();
    return events;
}

bool DBLayer::insertEvent(const Event &event)
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法插入事项";
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("INSERT INTO EventTable (userId, title, eventDate, eventTime, remindFlag, remindTime, isExpiredFlag, isDeleted) "
                  "VALUES (:userId, :title, :eventDate, :eventTime, :remindFlag, :remindTime, :isExpiredFlag, :isDeleted)");
    query.bindValue(":userId", event.userId);
    query.bindValue(":title", event.title);
    query.bindValue(":eventDate", event.eventDate.toString());
    query.bindValue(":eventTime", event.eventTime.toString());
    query.bindValue(":remindFlag", event.remindFlag ? 1 : 0);
    query.bindValue(":remindTime", event.remindTime.toString());
    query.bindValue(":isExpiredFlag", event.isExpiredFlag ? 1 : 0);
    query.bindValue(":isDeleted", event.isDeleted ? 1 : 0);

    if (!query.exec())
    {
        qDebug() << "插入事项失败：" << query.lastError().text();
        closeDatabase();
        return false;
    }
    closeDatabase();
    return true;
}

bool DBLayer::updateEvent(const Event &event)
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法更新事项";
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("UPDATE EventTable SET userId = :userId, title = :title, eventDate = :eventDate, "
                  "eventTime = :eventTime, remindFlag = :remindFlag, remindTime = :remindTime, "
                  "isExpiredFlag = :isExpiredFlag, isDeleted = :isDeleted WHERE eventId = :eventId");
    query.bindValue(":eventId", event.eventId);
    query.bindValue(":userId", event.userId);
    query.bindValue(":title", event.title);
    query.bindValue(":eventDate", event.eventDate.toString());
    query.bindValue(":eventTime", event.eventTime.toString());
    query.bindValue(":remindFlag", event.remindFlag ? 1 : 0);
    query.bindValue(":remindTime", event.remindTime.toString());
    query.bindValue(":isExpiredFlag", event.isExpiredFlag ? 1 : 0);
    query.bindValue(":isDeleted", event.isDeleted ? 1 : 0);

    if (!query.exec())
    {
        qDebug() << "更新事项失败：" << query.lastError().text();
        closeDatabase();
        return false;
    }
    closeDatabase();
    return true;
}

bool DBLayer::deleteEvent(std::size_t event_id)
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法删除事项";
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("UPDATE EventTable SET isDeleted = 1 WHERE eventId = :eventId");
    query.bindValue(":eventId", static_cast<int>(event_id));
    if (!query.exec())
    {
        qDebug() << "删除事项失败：" << query.lastError().text();
        closeDatabase();
        return false;
    }
    closeDatabase();
    return true;
}

void DBLayer::insertPomoRecord(Pomodoro pomo)
{
}

DateRecord DBLayer::getRecordbyDate(Date date)
{
    return DateRecord();
}
