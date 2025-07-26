#include "DBLayer.h"

#include <utility>

#include "Utility.h"

DBLayer::DBLayer(std::string db_file_name) : db_file_name_(std::move(db_file_name))
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

    // 创建 PomodoroTable
    QString pomoSql = "CREATE TABLE IF NOT EXISTS PomodoroTable ("
                      "pomoId INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "userId INTEGER, "
                      "recordDate TEXT, "
                      "recordTime TEXT, "
                      "record TEXT)";
    if (!query.exec(pomoSql))
    {
        qDebug() << "PomodoroTable 创建失败：" << query.lastError().text();
    }
    else
    {
        qDebug() << "PomodoroTable 创建成功";
    }

    // 创建 DateRecordTable 用于存储习惯打卡记录
    QString dateRecordSql = "CREATE TABLE IF NOT EXISTS DateRecordTable ("
                            "recordId INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "habitId INTEGER, "
                            "userId INTEGER, "
                            "recordDate TEXT, "
                            "recordTime TEXT, "
                            "FOREIGN KEY(habitId) REFERENCES HabitTable(habitId))";
    if (!query.exec(dateRecordSql))
    {
        qDebug() << "DateRecordTable 创建失败：" << query.lastError().text();
    }
    else
    {
        qDebug() << "DateRecordTable 创建成功";
    }

    // 创建 UserSettingsTable 用于存储用户设置
    QString userSettingsSql = "CREATE TABLE IF NOT EXISTS UserSettingsTable ("
                              "userId INTEGER PRIMARY KEY, "
                              "nickname TEXT, "
                              "currentSkin TEXT, "
                              "ddlReminderEnabled INTEGER, "
                              "ddlReminderEmail TEXT, "
                              "lastLoginTime TEXT, "
                              "isLoggedIn INTEGER)";
    if (!query.exec(userSettingsSql))
    {
        qDebug() << "UserSettingsTable 创建失败：" << query.lastError().text();
    }
    else
    {
        qDebug() << "UserSettingsTable 创建成功";
    }

    // 构造函数中初始化完后关闭数据库
    closeDatabase();
}

DBLayer::~DBLayer()
{
    closeDatabase();
}

bool DBLayer::openDatabase() const
{
    if (db_.isOpen())
    {
        return true;
    }
    return db_.open();
}

void DBLayer::closeDatabase() const
{
    if (db_.isOpen())
    {
        db_.close();
    }
}

std::vector<Habit> DBLayer::getHabitLists() const
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
        Habit habit{
            query.value("habitId").toULongLong(),
            query.value("userId").toULongLong(),
            query.value("name").toString().toStdString(),
            query.value("targetCount").toULongLong(),
            dateFromString(query.value("startDate").toString().toStdString()),
            dateFromString(query.value("endDate").toString().toStdString()),
            query.value("isActive").toBool(),
            query.value("isDeleted").toBool()};
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
    query.bindValue(":userId", habit.user_id);
    query.bindValue(":name", QString::fromStdString(habit.name));
    query.bindValue(":targetCount", habit.target_count);
    query.bindValue(":startDate", QString::fromStdString(toString(habit.start_date)));
    query.bindValue(":endDate", QString::fromStdString(toString(habit.end_date)));
    query.bindValue(":isActive", habit.is_active ? 1 : 0);
    query.bindValue(":isDeleted", habit.is_deleted ? 1 : 0);

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
    query.bindValue(":habitId", habit.habit_id);
    query.bindValue(":userId", habit.user_id);
    query.bindValue(":name", QString::fromStdString(habit.name));
    query.bindValue(":targetCount", habit.target_count);
    query.bindValue(":startDate", QString::fromStdString(toString(habit.start_date)));
    query.bindValue(":endDate", QString::fromStdString(toString(habit.end_date)));
    query.bindValue(":isActive", habit.is_active ? 1 : 0);
    query.bindValue(":isDeleted", habit.is_deleted ? 1 : 0);

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

bool DBLayer::setInactiveHabit(std::size_t habit_id)
{
    return false;
}

bool DBLayer::setActiveHabit(std::size_t habit_id)
{
    return false;
}

bool DBLayer::insertHabitRecord(const Habit &habit)
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法插入习惯打卡记录";
        return false;
    }

    QSqlQuery query(db_);
    // 使用 QDateTime 获取当前日期和时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dateStr = currentDateTime.toString("yyyy-MM-dd"); // 格式化为日期字符串
    QString timeStr = currentDateTime.toString("HH:mm:ss");   // 格式化为时间字符串

    query.prepare("INSERT INTO DateRecordTable (habitId, userId, recordDate, recordTime) "
                  "VALUES (:habitId, :userId, :recordDate, :recordTime)");
    query.bindValue(":habitId", habit.habit_id);
    query.bindValue(":userId", habit.user_id);
    query.bindValue(":recordDate", dateStr);
    query.bindValue(":recordTime", timeStr);

    if (!query.exec())
    {
        qDebug() << "插入习惯打卡记录失败：" << query.lastError().text();
        closeDatabase();
        return false;
    }
    closeDatabase();
    return true;
}

std::vector<Event> DBLayer::getEventLists() const
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
        Event event{
            query.value("eventId").toULongLong(),
            query.value("userId").toULongLong(),
            query.value("title").toString().toStdString(),
            dateFromString(query.value("eventDate").toString().toStdString()),
            timeFromString(query.value("eventTime").toString().toStdString()),
            query.value("remindFlag").toBool(),
            timeFromString(query.value("remindTime").toString().toStdString()),
            query.value("isExpiredFlag").toBool(),
            query.value("isDeleted").toBool()};
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
    query.bindValue(":userId", event.user_id);
    query.bindValue(":title", QString::fromStdString(event.title));
    query.bindValue(":eventDate", QString::fromStdString(toString(event.event_date)));
    query.bindValue(":eventTime", QString::fromStdString(toString(event.event_time)));
    query.bindValue(":remindFlag", event.remind_flag);
    query.bindValue(":remindTime", QString::fromStdString(toString(event.remind_time)));
    query.bindValue(":isExpiredFlag", event.is_expired_flag);
    query.bindValue(":isDeleted", event.is_deleted);

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
    query.bindValue(":eventId", event.event_id);
    query.bindValue(":userId", event.user_id);
    query.bindValue(":title", QString::fromStdString(event.title));
    query.bindValue(":eventDate", QString::fromStdString(toString(event.event_date)));
    query.bindValue(":eventTime", QString::fromStdString(toString(event.event_time)));
    query.bindValue(":remindFlag", event.remind_flag);
    query.bindValue(":remindTime", QString::fromStdString(toString(event.remind_time)));
    query.bindValue(":isExpiredFlag", event.is_expired_flag);
    query.bindValue(":isDeleted", event.is_deleted);

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
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法插入番茄钟记录";
        return;
    }
    Utility utility;

    QSqlQuery query(db_);
    query.prepare("INSERT INTO PomodoroTable (userId, recordDate, recordTime, record) "
                  "VALUES (:userId, :recordDate, :recordTime, :record)");
    query.bindValue(":userId", 0); // 假设 userId 为 0
    query.bindValue(":recordDate", QString::fromStdString(toString(utility.getCurrentTimeStamp().first)));
    query.bindValue(":recordTime", QString::fromStdString(toString(pomo.pomodoro_time)));
    query.bindValue(":record", QString::fromStdString(pomo.record));

    if (!query.exec())
    {
        qDebug() << "插入番茄钟记录失败：" << query.lastError().text();
    }
    closeDatabase();
}

DateRecord DBLayer::getRecordbyDate(Date date) const
{
    std::vector<std::pair<Time, Habit>> habit_records;
    std::vector<std::pair<Time, Pomodoro>> pomodoro_records;
    std::vector<std::pair<Time, Event>> event_records;

    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法获取指定日期的记录";
        return DateRecord(habit_records, pomodoro_records, event_records);
    }

    // 查询指定日期的习惯打卡记录
    QString dateStr = QString::fromStdString(toString(date));
    QSqlQuery habitQuery(db_);
    habitQuery.prepare("SELECT dr.*, ht.name, ht.targetCount, ht.startDate, ht.endDate, ht.isActive, ht.isDeleted "
                       "FROM DateRecordTable dr "
                       "JOIN HabitTable ht ON dr.habitId = ht.habitId "
                       "WHERE dr.recordDate = :date");
    habitQuery.bindValue(":date", dateStr);

    if (!habitQuery.exec())
    {
        qDebug() << "查询指定日期的习惯打卡记录失败：" << habitQuery.lastError().text();
    }
    else
    {
        while (habitQuery.next())
        {
            Habit habit{
                habitQuery.value("habitId").toULongLong(),
                habitQuery.value("userId").toULongLong(),
                habitQuery.value("name").toString().toStdString(),
                habitQuery.value("targetCount").toULongLong(),
                dateFromString(habitQuery.value("startDate").toString().toStdString()),
                dateFromString(habitQuery.value("endDate").toString().toStdString()),
                habitQuery.value("isActive").toBool(),
                habitQuery.value("isDeleted").toBool()};
            Time time = timeFromString(habitQuery.value("recordTime").toString().toStdString());
            habit_records.emplace_back(time, habit);
        }
    }

    // 查询指定日期的事项记录
    QSqlQuery eventQuery(db_);
    eventQuery.prepare("SELECT * FROM EventTable WHERE eventDate = :date AND isDeleted = 0");
    eventQuery.bindValue(":date", dateStr);

    if (!eventQuery.exec())
    {
        qDebug() << "查询指定日期的事项记录失败：" << eventQuery.lastError().text();
    }
    else
    {
        while (eventQuery.next())
        {
            Event event{
                eventQuery.value("eventId").toULongLong(),
                eventQuery.value("userId").toULongLong(),
                eventQuery.value("title").toString().toStdString(),
                dateFromString(eventQuery.value("eventDate").toString().toStdString()),
                timeFromString(eventQuery.value("eventTime").toString().toStdString()),
                eventQuery.value("remindFlag").toBool(),
                timeFromString(eventQuery.value("remindTime").toString().toStdString()),
                eventQuery.value("isExpiredFlag").toBool(),
                eventQuery.value("isDeleted").toBool()};

            // 使用事项的时间作为记录时间
            Time time = event.event_time;
            event_records.emplace_back(time, event);
        }
    }

    // 查询指定日期的番茄钟使用记录
    QSqlQuery pomodoroQuery(db_);
    pomodoroQuery.prepare("SELECT * FROM PomodoroTable WHERE recordDate = :date");
    pomodoroQuery.bindValue(":date", dateStr);

    if (!pomodoroQuery.exec())
    {
        qDebug() << "查询指定日期的番茄钟使用记录失败：" << pomodoroQuery.lastError().text();
    }
    else
    {
        while (pomodoroQuery.next())
        {
            // 提取数据库中的字段
            std::size_t id = pomodoroQuery.value("pomoId").toULongLong();
            QString recordTimeStr = pomodoroQuery.value("recordTime").toString();
            std::string record = pomodoroQuery.value("record").toString().toStdString();

            // 将时间字符串转换为 Time 对象
            Time time = timeFromString(recordTimeStr.toStdString());

            // 构造 Pomodoro 对象
            Pomodoro pomodoro(id, time, record);

            pomodoro_records.emplace_back(time, pomodoro);
        }
    }

    closeDatabase();
    return DateRecord(habit_records, pomodoro_records, event_records);
}

int DBLayer::getHabitIDMax()
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法获取最大习惯ID";
        return 0;
    }

    QSqlQuery query(db_);
    QString sql = "SELECT MAX(habitId) as maxId FROM HabitTable";
    if (!query.exec(sql))
    {
        qDebug() << "查询最大习惯ID失败：" << query.lastError().text();
        closeDatabase();
        return 0;
    }

    int maxId = 0;
    if (query.next())
    {
        maxId = query.value("maxId").toInt();
    }

    closeDatabase();
    return maxId;
}

int DBLayer::getEventIDMax()
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法获取最大事项ID";
        return 0;
    }

    QSqlQuery query(db_);
    QString sql = "SELECT MAX(eventId) as maxId FROM EventTable";
    if (!query.exec(sql))
    {
        qDebug() << "查询最大事项ID失败：" << query.lastError().text();
        closeDatabase();
        return 0;
    }

    int maxId = 0;
    if (query.next())
    {
        maxId = query.value("maxId").toInt();
    }

    closeDatabase();
    return maxId;
}

int DBLayer::getPomoIDMax()
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法获取最大番茄钟ID";
        return 0;
    }

    QSqlQuery query(db_);
    QString sql = "SELECT MAX(pomoId) as maxId FROM PomodoroTable";
    if (!query.exec(sql))
    {
        qDebug() << "查询最大番茄钟ID失败：" << query.lastError().text();
        closeDatabase();
        return 0;
    }

    int maxId = 0;
    if (query.next())
    {
        maxId = query.value("maxId").toInt();
    }

    closeDatabase();
    return maxId;
}

UserSettings DBLayer::getUserSettings(std::size_t user_id) const
{
    UserSettings settings;
    settings.user_id = user_id;
    settings.nickname = "";
    settings.current_skin = "default";
    settings.ddl_reminder_enabled = false;
    settings.ddl_reminder_email = "";
    settings.last_login_time = "";
    settings.is_logged_in = false;

    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法获取用户设置";
        return settings;
    }

    QSqlQuery query(db_);
    query.prepare("SELECT * FROM UserSettingsTable WHERE userId = :userId");
    query.bindValue(":userId", static_cast<int>(user_id));

    if (!query.exec())
    {
        qDebug() << "查询用户设置失败：" << query.lastError().text();
        closeDatabase();
        return settings;
    }

    if (query.next())
    {
        settings.user_id = query.value("userId").toULongLong();
        settings.nickname = query.value("nickname").toString().toStdString();
        settings.current_skin = query.value("currentSkin").toString().toStdString();
        settings.ddl_reminder_enabled = query.value("ddlReminderEnabled").toBool();
        settings.ddl_reminder_email = query.value("ddlReminderEmail").toString().toStdString();
        settings.last_login_time = query.value("lastLoginTime").toString().toStdString();
        settings.is_logged_in = query.value("isLoggedIn").toBool();
    }

    closeDatabase();
    return settings;
}

bool DBLayer::updateUserSettings(const UserSettings &settings)
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法更新用户设置";
        return false;
    }

    QSqlQuery checkQuery(db_);
    checkQuery.prepare("SELECT COUNT(*) FROM UserSettingsTable WHERE userId = :userId");
    checkQuery.bindValue(":userId", static_cast<int>(settings.user_id));

    if (!checkQuery.exec())
    {
        qDebug() << "检查用户设置记录失败：" << checkQuery.lastError().text();
        closeDatabase();
        return false;
    }

    bool recordExists = false;
    if (checkQuery.next())
    {
        recordExists = checkQuery.value(0).toInt() > 0;
    }

    QSqlQuery query(db_);
    if (recordExists)
    {
        // 更新现有记录
        query.prepare("UPDATE UserSettingsTable SET "
                      "nickname = :nickname, "
                      "currentSkin = :currentSkin, "
                      "ddlReminderEnabled = :ddlReminderEnabled, "
                      "ddlReminderEmail = :ddlReminderEmail, "
                      "lastLoginTime = :lastLoginTime, "
                      "isLoggedIn = :isLoggedIn "
                      "WHERE userId = :userId");
    }
    else
    {
        // 插入新记录
        query.prepare("INSERT INTO UserSettingsTable "
                      "(userId, nickname, currentSkin, ddlReminderEnabled, ddlReminderEmail, lastLoginTime, isLoggedIn) "
                      "VALUES "
                      "(:userId, :nickname, :currentSkin, :ddlReminderEnabled, :ddlReminderEmail, :lastLoginTime, :isLoggedIn)");
    }

    query.bindValue(":userId", static_cast<int>(settings.user_id));
    query.bindValue(":nickname", QString::fromStdString(settings.nickname));
    query.bindValue(":currentSkin", QString::fromStdString(settings.current_skin));
    query.bindValue(":ddlReminderEnabled", settings.ddl_reminder_enabled ? 1 : 0);
    query.bindValue(":ddlReminderEmail", QString::fromStdString(settings.ddl_reminder_email));
    query.bindValue(":lastLoginTime", QString::fromStdString(settings.last_login_time));
    query.bindValue(":isLoggedIn", settings.is_logged_in ? 1 : 0);

    if (!query.exec())
    {
        qDebug() << "更新用户设置失败：" << query.lastError().text();
        closeDatabase();
        return false;
    }

    closeDatabase();
    return true;
}

std::size_t DBLayer::getCurrentUserID() const
{
    std::size_t currentUserId = 0;

    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法获取当前用户ID";
        return currentUserId;
    }

    QSqlQuery query(db_);
    query.prepare("SELECT userId FROM UserSettingsTable WHERE isLoggedIn = 1 LIMIT 1");

    if (!query.exec())
    {
        qDebug() << "查询当前用户ID失败：" << query.lastError().text();
        closeDatabase();
        return currentUserId;
    }

    if (query.next())
    {
        currentUserId = query.value("userId").toULongLong();
    }

    closeDatabase();
    return currentUserId;
}

bool DBLayer::setInactiveHabit(std::size_t habit_id)
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法停用习惯";
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("UPDATE HabitTable SET isActive = 0 WHERE habitId = :habitId AND isDeleted = 0");
    query.bindValue(":habitId", static_cast<int>(habit_id));

    if (!query.exec())
    {
        qDebug() << "停用习惯失败：" << query.lastError().text();
        closeDatabase();
        return false;
    }

    // 检查是否有行被更新
    if (query.numRowsAffected() <= 0)
    {
        qDebug() << "停用习惯失败：未找到指定ID的习惯或习惯已被删除";
        closeDatabase();
        return false;
    }

    closeDatabase();
    return true;
}

bool DBLayer::setActiveHabit(std::size_t habit_id)
{
    if (!openDatabase())
    {
        qDebug() << "数据库打开失败，无法启用习惯";
        return false;
    }

    QSqlQuery query(db_);
    query.prepare("UPDATE HabitTable SET isActive = 1 WHERE habitId = :habitId AND isDeleted = 0");
    query.bindValue(":habitId", static_cast<int>(habit_id));

    if (!query.exec())
    {
        qDebug() << "启用习惯失败：" << query.lastError().text();
        closeDatabase();
        return false;
    }

    // 检查是否有行被更新
    if (query.numRowsAffected() <= 0)
    {
        qDebug() << "启用习惯失败：未找到指定ID的习惯或习惯已被删除";
        closeDatabase();
        return false;
    }

    closeDatabase();
    return true;
}
