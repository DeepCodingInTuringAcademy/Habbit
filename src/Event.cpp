#include "Event.h"
#include <utility>


Event::Event(const std::size_t event_id,
             const std::size_t user_id,
             std::string title,
             const Date event_date,
             const Time event_time,
             const bool remind_flag,
             const bool is_expired_flag,
             const bool is_deleted)
           : event_id(event_id),
             user_id(user_id),
             title(std::move(title)),
             event_date(event_date),
             event_time(event_time),
             remind_flag(remind_flag),
             is_expired_flag(is_expired_flag),
             is_deleted(is_deleted) {}
