#pragma once

#include "sead/task.h"

class CourseTask : public sead::CalculateTask {
    SEAD_SINGLETON_TASK(CourseTask);

public:
    static sead::TaskBase* construct(const sead::TaskConstructArg& arg);

    void incrementCoinCounter(u32 unk, u32 coinCount);
};
