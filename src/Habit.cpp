#include "Habit.h"

#include <utility>


Habit::Habit(const std::size_t habit_id,
             const std::size_t user_id,
             std::string name,
             const std::size_t target_count,
             const Date start_date,
             const Date end_date,
             const bool is_active,
             const bool is_deleted)
           : habit_id(habit_id),
             user_id(user_id),
             name(std::move(name)),
             target_count(target_count),
             start_date(start_date),
             end_date(end_date),
             is_active(is_active),
             is_deleted(is_deleted) {}
