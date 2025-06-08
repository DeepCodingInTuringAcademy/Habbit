#include "DBLayer.h"


bool DBLayer::openDatabase() const
{
    return false;
}

void DBLayer::closeDatabase() const
{
}

DBLayer::DBLayer(std::string db_file_name)
{
}

std::vector<Habit> DBLayer::getHabitLists() const
{
    return std::vector<Habit>();
}

bool DBLayer::insertHabit(const Habit& habit)
{
    return false;
}

bool DBLayer::updateHabit(std::size_t habit_id)
{
    return false;
}

bool DBLayer::deleteHabit(std::size_t habit_id)
{
    return false;
}

std::vector<Event> DBLayer::getEventLists() const
{
    return std::vector<Event>();
}

bool DBLayer::insertEvent(const Event& event)
{
    return false;
}

bool DBLayer::updateEvent(std::size_t event_id)
{
    return false;
}

bool DBLayer::deleteEvent(std::size_t event_id)
{
    return false;
}
