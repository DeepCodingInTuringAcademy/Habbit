#ifndef DBLAYER_H
#define DBLAYER_H

#include <QSqlDatabase>
#include <vector>
#include "Event.h"
#include "Habit.h"

class DBLayer
{
    QString db_file_name_;
    mutable QSqlDatabase db_;

    bool openDatabase() const;

    void closeDatabase() const;

public:
    explicit DBLayer(QString db_file_name);

    std::vector<Habit> getHabitLists() const;

    bool insertHabit(const Habit& habit);

    bool updateHabit(std::size_t habit_id);

    bool deleteHabit(std::size_t habit_id);

    std::vector<Event> getEventLists() const;

    bool insertEvent(const Event& event);

    bool updateEvent(std::size_t event_id);

    bool deleteEvent(std::size_t event_id);
};

#endif // DBLAYER_H