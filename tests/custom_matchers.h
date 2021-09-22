#pragma once

#include <gmock/gmock.h>

MATCHER(EndsWithNumbersAfterUnderscore, "")
{
    for (auto it = arg.rbegin(); *it != '_'; it++)
        if (!isdigit(*it))
            return false;
    return true;
}
