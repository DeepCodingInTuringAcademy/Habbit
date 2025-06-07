#ifndef EVENT_H
#define EVENT_H
#include "Time.h"


class Event
{
    std::size_t event_id;
    std::size_t user_id;
    std::string title;
    Date event_date;
    Time event_time;
    bool remind_flag;
    Time remind_time;
    bool is_expired_flag;
    bool is_deleted;

    explicit Event(std::size_t event_id,
                   std::size_t user_id,
                   std::string  title,
                   Date event_date,
                   Time event_time,
                   bool remind_flag,
                   bool is_expired_flag,
                   bool is_deleted);
};

#endif //EVENT_H
