#include "Pomodoro.h"

#include <utility>

Pomodoro::Pomodoro(
    const std::size_t id,
    const Time pomodoro_duration, std::string record
    ) :
    id(id),
    pomodoro_duration(pomodoro_duration),
    record(std::move(record))
{
}

