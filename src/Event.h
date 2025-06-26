#ifndef EVENT_H
#define EVENT_H
#include "Times.h"

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

public:
    explicit Event(std::size_t event_id = 0, //NOLINT
                   std::size_t user_id = 0,
                   std::string title = "",
                   Date event_date = {},
                   Time event_time = {},
                   bool remind_flag = false,
                   Time remind_time = {},
                   bool is_expired_flag = false,
                   bool is_deleted = false);

    //添加公共getter方法
    const std::string& getTitle() const{
        return title;
    }

    const Date& getevent_date() const{
        return event_date;
    }

    const Time& getevent_time() const{
        return event_time;
    }

};


#endif //EVENT_H
