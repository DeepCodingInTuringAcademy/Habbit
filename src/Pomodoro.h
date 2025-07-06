#ifndef POMODORO_H
#define POMODORO_H
#include "Times.h"


struct Pomodoro
{
    std::size_t id;
    Time pomodoro_time;
    std::string record;

    /**
     *
     * @param id
     * @param pomodoro_time
     * @param record
     */
    explicit Pomodoro(
        std::size_t id,
        Time pomodoro_time,
        std::string record
        );
};

#endif //POMODORO_H
