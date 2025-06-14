#ifndef DBLAYER_H
#define DBLAYER_H

#include <QSqlDatabase>
#include <vector>
#include "Event.h"
#include "Habit.h"

class DBLayer
{
    std::string db_file_name_;
    QSqlDatabase db_;

    [[nodiscard]] bool openDatabase() const;

    void closeDatabase() const;

public:
    explicit DBLayer(std::string db_file_name = "");

    [[nodiscard]] std::vector<Habit> getHabitLists() const;

    bool insertHabit(const Habit& habit);

    bool updateHabit(std::size_t habit_id);

    bool deleteHabit(std::size_t habit_id);

    [[nodiscard]] std::vector<Event> getEventLists() const;

    bool insertEvent(const Event& event);

    bool updateEvent(std::size_t event_id);

    bool deleteEvent(std::size_t event_id);
};

#endif // DBLAYER_H