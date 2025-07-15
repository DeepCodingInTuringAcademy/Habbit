#include "ServiceLayer.h"

bool ServiceLayer::insertHabit(std::string name, const Date& start_date, const Date& end_date, int times_per_day)
{
    return false;
}

bool ServiceLayer::updateHabit(int habit_id, const Date& start_date, const Date& end_date, int times_per_day,
    bool active_flag)
{
    return false;
}

bool ServiceLayer::deleteHabit(int habit_id)
{
    return false;
}

bool ServiceLayer::insertEvent(std::string name, const Date& event_date, const Time& event_time, bool remind_flag,
    const Time& remind_time)
{
    return false;
}

bool ServiceLayer::updateEvent(int event_id, std::string title, const Date& event_date, const Time& event_time,
    bool remind_flag, const Time& remind_time)
{
    return false;
}

bool ServiceLayer::deleteEvent(int event_id)
{
    return false;
}

bool ServiceLayer::insertHabitRecord(const Habit& habit)
{
    return false;
}

std::vector<Habit> ServiceLayer::getActiveHabits() const
{
    return {};
}

std::vector<Habit> ServiceLayer::getInactiveHabits() const
{
    return {};
}

std::vector<Event> ServiceLayer::getActiveEvents() const
{
    return {};
}

std::vector<Event> ServiceLayer::getExpiredEvents() const
{
    return {};
}

bool ServiceLayer::pomodoroTick(const Pomodoro& pomodoro, const Time& count_time)
{
    auto now_time = getCurrentTimeStamp().second;
    if ((pomodoro.pomodoro_time + count_time) < now_time)
    {
        // TODO: insertPomodoroRecord
        return false;
    }
    return true;
}

std::vector<std::pair<std::size_t, std::size_t>> ServiceLayer::getHabitRecordsByDate(const Date& date) const
{
    return {};
}

DateRecord ServiceLayer::getAllRecordsByDate(const Date& date) const
{
    // return db_layer.getRecordbyDate(date);
}

std::pair<Date, Time> ServiceLayer::getCurrentTimeStamp() const
{
    return {};
}

Habit ServiceLayer::getHabitByID(int habit_id) const
{
    return Habit();
}

Event ServiceLayer::getEventByID(int event_id) const
{
    return Event();
}

void ServiceLayer::init()
{
    DBLayer db_layer("HabbitDatabase.db");
}

