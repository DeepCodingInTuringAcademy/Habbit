#ifndef HABIT_H
#define HABIT_H
#include "Times.h"


struct Habit
{
    std::size_t habit_id;
    std::size_t user_id;
    std::string name;
    std::size_t target_count;
    Date start_date;
    Date end_date;
    bool is_active;
    bool is_deleted;

    explicit Habit(std::size_t habit_id = 0,
                   std::size_t user_id = 0,
                   std::string name = "",
                   std::size_t target_count = 0,
                   Date start_date = Date(),
                   Date end_date = Date(),
                   bool is_active = false,
                   bool is_deleted = false);
};

#endif //HABIT_H
