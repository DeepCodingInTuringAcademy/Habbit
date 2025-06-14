#ifndef SERVICELAYER_H
#define SERVICELAYER_H
#include "DateRecord.h"
#include "DBLayer.h"
#include "Pomodoro.h"


class ServiceLayer
{
    DBLayer db_layer;

public:
    bool insertHabit(std::string name, const Date& start_date, const Date& end_date, int times_per_day);

    bool updateHabit(int habit_id, const Date& start_date, const Date& end_date, int times_per_day, bool active_flag);

    bool deleteHabit(int habit_id);

    bool insertEvent(std::string name, const Date& event_date, const Time& event_time, bool remind_flag, const Time& remind_time);

    bool updateEvent(int event_id, std::string title, const Date& event_date, const Time& event_time, bool remind_flag, const Time& remind_time);

    bool deleteEvent(int event_id);

    bool insertHabitRecord(const Habit& habit);

    [[nodiscard]] std::vector<Habit> getActiveHabits() const;

    [[nodiscard]] std::vector<Habit> getInactiveHabits() const;

    [[nodiscard]] std::vector<Event> getActiveEvents() const;

    [[nodiscard]] std::vector<Event> getExpiredEvents() const;

    bool pomodoroTick(const Pomodoro& pomodoro, const Time& count_time);

    [[nodiscard]] std::vector<std::pair<std::size_t, std::size_t>> getHabitRecordsByDate(const Date& date) const;

    [[nodiscard]] DateRecord getAllRecordsByDate(const Date& date) const;

    [[nodiscard]] std::pair<Date, Time> getCurrentTimeStamp() const;

    [[nodiscard]] Habit getHabitByID(int habit_id) const;

    [[nodiscard]] Event getEventByID(int event_id) const;

    void init();
};

#endif //SERVICELAYER_H
