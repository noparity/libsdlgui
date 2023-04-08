#include "stdafx.h"
#include "helpers.hpp"

std::string SafeGetEnv(const std::string& name)
{
    size_t reqCount;
    char buffer[128];
    auto err = getenv_s(&reqCount, buffer, 128, name.c_str());
    assert(err == 0);

    return std::string(buffer);
}
