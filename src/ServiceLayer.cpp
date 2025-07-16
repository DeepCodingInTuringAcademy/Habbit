#include "ServiceLayer.h"

#include <utility>

bool ServiceLayer::insertHabit(const std::string& name, const Date& start_date, const Date& end_date, std::size_t times_per_day)
{
    auto times = this->getCurrentTimeStamp();
    Date cur_date = times.first;
    Time cur_time = times.second;
    if (name.empty() || times_per_day <= 0 || start_date > end_date || end_date < cur_date)
    {
        return false;
    }
    this->db_layer.insertHabit(Habit{ 0, 0, name, times_per_day,
                                        start_date, end_date, true, false });

    return true;
}

bool ServiceLayer::updateHabit(std::size_t habit_id, const Date& start_date, const Date& end_date, std::size_t times_per_day,
                               bool active_flag)
{
    auto times = this->getCurrentTimeStamp();
    Date cur_date = times.first;
    Time cur_time = times.second;
    if (times_per_day <= 0 || start_date > end_date || end_date < cur_date)
    {
        return false;
    }
    Habit habit = this->getHabitByID(habit_id);
    habit.start_date = start_date;
    habit.end_date = end_date;
    habit.target_count = times_per_day;
    habit.is_active = active_flag;
    this->db_layer.updateHabit(habit);

    return true;
}

bool ServiceLayer::deleteHabit(std::size_t habit_id)
{
    this->db_layer.deleteHabit(habit_id);
    return true;
}

bool ServiceLayer::insertEvent(const std::string& name, const Date& event_date, const Time& event_time, bool remind_flag,
    const Time& remind_time)
{
    auto times = this->getCurrentTimeStamp();
    Date cur_date = times.first;
    Time cur_time = times.second;
    if (name.empty() || remind_time > event_time || cur_date > event_date)
    {
        return false;
    }
    this->db_layer.insertEvent(Event{ 0, 0, name,
                                        event_date, event_time, remind_flag, remind_time });

    return true;
}

bool ServiceLayer::updateEvent(std::size_t event_id, std::string title, const Date& event_date, const Time& event_time,
                               bool remind_flag, const Time& remind_time)
{
    auto times = this->getCurrentTimeStamp();
    Date cur_date = times.first;
    Time cur_time = times.second;
    if (remind_time > event_time || cur_date > event_date)
    {
        return false;
    }
    Event event = this->getEventByID(event_id);
    event.title = std::move(title);
    event.event_date = event_date;
    event.event_time = event_time;
    event.remind_flag = remind_flag;
    event.remind_time = remind_time;
    this->db_layer.updateEvent(event);

    return true;
}

bool ServiceLayer::deleteEvent(int event_id)
{
    this->db_layer.deleteEvent(event_id);
    return true;
}

bool ServiceLayer::insertHabitRecord(const Habit& habit)
{
    this->db_layer.insertHabit(habit);
    return true;
}

std::vector<Habit> ServiceLayer::getActiveHabits() const
{
    std::vector<Habit> all_habits = this->db_layer.getHabitLists();
    std::vector<Habit> active_habits;

    for (const auto& habit : all_habits) {
        if (habit.is_active && !habit.is_deleted) {
            active_habits.push_back(habit);
        }
    }

    return active_habits;
}

std::vector<Habit> ServiceLayer::getInactiveHabits() const
{
    std::vector<Habit> all_habits = this->db_layer.getHabitLists();
    std::vector<Habit> inactive_habits;

    for (const auto& habit : all_habits) {
        if (!habit.is_active || habit.is_deleted) {
            inactive_habits.push_back(habit);
        }
    }

    return inactive_habits;
}

std::vector<Event> ServiceLayer::getActiveEvents() const
{
    std::vector<Event> all_events = this->db_layer.getEventLists();
    std::vector<Event> active_events;

    for (const auto& event : all_events) {
        if (!event.is_expired_flag && !event.is_deleted) {
            active_events.push_back(event);
        }
    }

    return active_events;
}

std::vector<Event> ServiceLayer::getExpiredEvents() const
{
    std::vector<Event> all_events = this->db_layer.getEventLists();
    std::vector<Event> expired_events;

    for (const auto& event : all_events) {
        if (event.is_expired_flag || event.is_deleted) {
            expired_events.push_back(event);
        }
    }

    return expired_events;
}

bool ServiceLayer::pomodoroTick(const Pomodoro& pomodoro, const Time& count_time)
{
    auto now_time = getCurrentTimeStamp().second;
    /*
    if ((pomodoro.pomodoro_time + count_time) < now_time)
    {
        db_layer.insertPomoRecord(pomodoro);
        return false;
    }
    */
    return true;
}

std::vector<std::pair<std::size_t, std::size_t>> ServiceLayer::getHabitRecordsByDate(const Date& date) const
{
    /*
    std::vector<std::pair<std::size_t, std::size_t>> stats;
    Date day{date.year(), date.month(), date.day()};

    static int days[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (date.month == 2 && ((date.year % 4 == 0 && date.year % 100 != 0) || (date.year % 400 == 0)))
        days[1] = 29;

    for (int d = 1; d <= days[date.month - 1]; ++d)
    {
        day.day() = d;
        std::size_t should = 0, actual = 0;
        for (const auto& h : db_layer.getHabitLists())
            if (h.start_date <= day && h.end_date >= day)
                should += h.target_count;

        actual = db_layer.getRecordbyDate(day).size();
        stats.emplace_back(actual, should);
    }
    return stats;
    */
    return {};
}

DateRecord ServiceLayer::getAllRecordsByDate(const Date& date)
{
    return db_layer.getRecordbyDate(date);
}

std::pair<Date, Time> ServiceLayer::getCurrentTimeStamp() const
{
    return {};
}

Habit ServiceLayer::getHabitByID(std::size_t habit_id) const
{
    return Habit();
}

Event ServiceLayer::getEventByID(std::size_t event_id) const
{
    return Event();
}

void ServiceLayer::init()
{
    DBLayer db_layer("HabbitDatabase.db");
}

