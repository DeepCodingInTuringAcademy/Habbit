#ifndef DBLAYER_H
#define DBLAYER_H
#include <string>
#include <vector>
#include "Event.h"
#include "Habit.h"
#include"sqlite3.h"


class DBLayer
{
    std::string db_file_name_;

    static int dataBaseOpen(std::string&& db_file_name);

    static void dataBaseClose(sqlite3* db);
public:
    explicit DBLayer(std::string&& db_file_name);

    std::vector<Habit> getHabitLists() const;

    bool insertHabit(const Habit& habit);

    bool updateHabit(std::size_t habit_id);

    bool deleteHabit(std::size_t habit_id);

    std::vector<Event> getEventLists() const;

    bool insertEvent(const Event& event);

    bool updateEvent(std::size_t event_id);

    bool deleteEvent(std::size_t event_id);
};

#endif //DBLAYER_H
