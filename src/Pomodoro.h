#ifndef POMODORO_H
#define POMODORO_H
#include "Time.h"


struct Pomodoro
{
    std::size_t id;
    Time pomodoro_time;
    std::string record;

    explicit Pomodoro(std::size_t id, Time pomodoro_time, std::string record);
};

#endif //POMODORO_H
