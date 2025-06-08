#ifndef DATERECORD_H
#define DATERECORD_H
#include <vector>
#include "Habit.h"
#include "Pomodoro.h"


struct DateRecord
{
    std::vector<std::pair<Time, Habit>> habit_records;
    std::vector<std::pair<Time, Pomodoro>> pomodoro_records;

    explicit DateRecord(const std::vector<std::pair<Time, Habit>>& habit_records = {},
                        const std::vector<std::pair<Time, Pomodoro>>& pomodoro_records = {});
};

#endif //DATERECORD_H
