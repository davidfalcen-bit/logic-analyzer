#pragma once
#include "config.hpp"
#include <cstdint>

enum class state_of_parse : uint8_t {
    PARSING_CHANNEL,
    PARSING_HZ,
    PARSING_SAMPLES,
};
logic_an_input parse_cin();