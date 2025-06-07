#include "DateRecord.h"


DateRecord::DateRecord(const std::vector<std::pair<Time, Habit>>& habit_records,
                       const std::vector<std::pair<Time, Pomodoro>>& pomodoro_records)
                     : habit_records(habit_records), pomodoro_records(pomodoro_records) {}
