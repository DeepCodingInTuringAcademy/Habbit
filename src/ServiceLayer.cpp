#include "ServiceLayer.h"

bool ServiceLayer::insertHabit(std::string name, const Date& start_date, const Date& end_date, int times_per_day)
{
}

bool ServiceLayer::updateHabit(int habit_id, const Date& start_date, const Date& end_date, int times_per_day,
    bool active_flag)
{
}

bool ServiceLayer::deleteHabit(int habit_id)
{
}

bool ServiceLayer::insertEvent(std::string name, const Date& event_date, const Time& event_time, bool remind_flag,
    const Time& remind_time)
{
}

bool ServiceLayer::updateEvent(int event_id, std::string title, const Date& event_date, const Time& event_time,
    bool remind_flag, const Time& remind_time)
{
}

bool ServiceLayer::deleteEvent(int event_id)
{
}

bool ServiceLayer::insertHabitRecord(const Habit& habit)
{
}

std::vector<Habit> ServiceLayer::getActiveHabits() const
{
}

std::vector<Habit> ServiceLayer::getInactiveHabits() const
{
}

std::vector<Event> ServiceLayer::getActiveEvents() const
{
}

std::vector<Event> ServiceLayer::getExpiredEvents() const
{
}

bool ServiceLayer::pomodoroTick(const Pomodoro& pomodoro, const Time& count_time)
{
}

std::vector<std::pair<std::size_t, std::size_t>> ServiceLayer::getHabitRecordsByDate(const Date& date) const
{
}

DateRecord ServiceLayer::getAllRecordsByDate(const Date& date) const
{
}

std::pair<Date, Time> ServiceLayer::getCurrentTimeStamp() const
{
}

Habit ServiceLayer::getHabitByID(int habit_id) const
{
}

Event ServiceLayer::getEventByID(int event_id) const
{
}

void ServiceLayer::init()
{
}
