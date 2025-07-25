#include "ServiceLayer.h"

#include <utility>

#include "Utility.h"

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

bool ServiceLayer::checkinHabit(const Habit& habit)
{
    this->db_layer.insertHabitRecord(habit);
    return true;
}

std::vector<Habit> ServiceLayer::getActiveHabits() const
{
    std::vector<Habit> all_habits = this->db_layer.getHabitLists();
    std::vector<Habit> active_habits;

    for (const auto& habit : all_habits)
    {
        if (habit.is_active && !habit.is_deleted)
        {
            active_habits.push_back(habit);
        }
    }

    return active_habits;
}

std::vector<Habit> ServiceLayer::getInactiveHabits() const
{
    std::vector<Habit> all_habits = this->db_layer.getHabitLists();
    std::vector<Habit> inactive_habits;

    for (const auto& habit : all_habits)
    {
        if (!habit.is_active || habit.is_deleted)
        {
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
    if ((pomodoro.pomodoro_time + count_time) < now_time)
    {
        db_layer.insertPomoRecord(pomodoro);
        return false;
    }
    return true;
}

std::vector<std::pair<std::size_t, std::size_t>> ServiceLayer::getHabitRecordsByDate(const Date& date) const
{
    std::vector<std::pair<std::size_t, std::size_t>> stats;

    auto y = date.year();
    auto m = date.month();

    unsigned days_in_month = static_cast<unsigned>
    (
        std::chrono::year_month_day_last{ y, std::chrono::month_day_last{m} }.day()
    );

    for (unsigned d = 1; d <= days_in_month; ++d)
    {
        Date current_day = Date{ y / m / std::chrono::day{d} };
        std::size_t should = 0, actual = 0;

        for (const auto& h : db_layer.getHabitLists())
        {
            if (h.start_date <= current_day && h.end_date >= current_day)
                should += h.target_count;
        }

        actual = db_layer.getRecordbyDate(current_day).getSize(); // 注意根据实际类型修改字段名
        stats.emplace_back(actual, should);
    }

    return stats;
}


DateRecord ServiceLayer::getAllRecordsByDate(const Date& date)
{
    // 获取数据库中的原始数据
    // return db_layer.getRecordbyDate(date);
    std::vector<std::pair<Time, Habit>> habit_records;
    std::vector<std::pair<Time, Pomodoro>> pomodoro_records;

    for (int i = 0; i < 3; ++i)
    {
        Time record_time{std::chrono::seconds{8 * 3600 + i * 4000}};
        Habit habit
        {
            (size_t)(100 + i),             // habitId
            1,                   // userId
            "测试习惯" + std::to_string(i + 1),
            5,                   // targetCount
            date,                // startDate
            date,                // endDate
            true,                // isActive
            false                // isDeleted
        };
        habit_records.emplace_back(record_time, habit);
    }

    for (int i = 0; i < 2; ++i)
    {
        Time record_time{std::chrono::seconds{14 * 3600 + i * 4000}};
        Pomodoro pomodoro
        {
            (size_t)(200 + i),             // pomoId
            record_time,
            "测试番茄钟记录 " + std::to_string(i + 1)
        };
        pomodoro_records.emplace_back(record_time, pomodoro);
    }
    return DateRecord(habit_records, pomodoro_records);
}

std::pair<Date, Time> ServiceLayer::getCurrentTimeStamp() const
{
    auto now_time=std::chrono::system_clock::now();
    auto local_time=std::chrono::current_zone()->to_local(now_time);

    auto time_since_midnight = local_time - std::chrono::floor<std::chrono::days>(local_time);
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_midnight);

    return
 {
        std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(local_time)),
        std::chrono::hh_mm_ss<std::chrono::seconds>(seconds)
    };
}

Habit ServiceLayer::getHabitByID(std::size_t habit_id) const
{
    for (const auto& habit : this->db_layer.getHabitLists())
    {
        if (habit.habit_id == habit_id)
        {
            return habit;
        }
    }

    return Habit();

}

Event ServiceLayer::getEventByID(std::size_t event_id) const
{
    for (const auto& event : this->db_layer.getEventLists())
    {
        if (event.event_id == event_id)
        {
            return event;
        }
    }
    return Event();

}

std::vector<Habit> ServiceLayer::getHabitsByDate(const QDate date) const
{
    std::vector<Habit> habits;
    for (const auto& habit : this->db_layer.getHabitLists())
    {
        if (habit.start_date <= Utility::convertQTDateToDate(date) &&
            Utility::convertQTDateToDate(date) <= habit.end_date)
        {
            habits.emplace_back(habit);
        }
    }
    return habits;
}

std::vector<Event> ServiceLayer::getEventsByDate(const QDate date) const
{
    std::vector<Event> events;
    for (const auto& event : this->db_layer.getEventLists())
    {
        if (Utility::convertQTDateToDate(date) == event.event_date)
        {
            events.emplace_back(event);
        }
    }
    return events;
}

void ServiceLayer::init()
{
}

