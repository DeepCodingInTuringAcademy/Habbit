#include "Pomodoro.h"

#include <utility>

Pomodoro::Pomodoro(
    const std::size_t id,
    const Time pomodoro_time, std::string record
    ) :
    id(id),
    pomodoro_time(pomodoro_time),
    record(std::move(record))
{
}
