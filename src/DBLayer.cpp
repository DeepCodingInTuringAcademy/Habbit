#include "DBLayer.h"

int DBLayer::dataBaseOpen(std::string&& db_file_name)
{
}

void DBLayer::dataBaseClose(sqlite3* db)
{
}

DBLayer::DBLayer(std::string&& db_file_name)
    : db_file_name_(std::move(db_file_name)) {}

std::vector<Habit> DBLayer::getHabitLists() const
{
}

bool DBLayer::insertHabit(const Habit& habit)
{
}

bool DBLayer::updateHabit(std::size_t habit_id)
{
}

bool DBLayer::deleteHabit(std::size_t habit_id)
{
}

std::vector<Event> DBLayer::getEventLists() const
{
}

bool DBLayer::insertEvent(const Event& event)
{
}

bool DBLayer::updateEvent(std::size_t event_id)
{
}

bool DBLayer::deleteEvent(std::size_t event_id)
{
}
