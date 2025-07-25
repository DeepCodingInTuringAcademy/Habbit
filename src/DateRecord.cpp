#include "DateRecord.h"

DateRecord::DateRecord(
    const std::vector<std::pair<Time, Habit>> &habit_records,
    const std::vector<std::pair<Time, Pomodoro>> &pomodoro_records,
    const std::vector<std::pair<Time, Event>> &event_records) : habit_records(habit_records),
                                                                pomodoro_records(pomodoro_records),
                                                                event_records(event_records)
{
}

std::size_t DateRecord::getSize() const
{
    return habit_records.size() + pomodoro_records.size() + event_records.size();
}
