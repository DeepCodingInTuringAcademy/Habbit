#include "Pomodoro.h"

#include <utility>


Pomodoro::Pomodoro(std::size_t id, Time pomodoro_time, std::string  record)
    : id(id), pomodoro_time(pomodoro_time), record(std::move(record)) {}
