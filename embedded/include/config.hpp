#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <format>
struct logic_an_input {
    uint8_t channel;
    uint32_t hz;
    uint32_t samples;
};

inline std::string from_hz_to_string(unsigned int hz) {
    if (hz >= 1'000'000) {
        return std::format("{:.2f} MHz", hz / 1'000'000.0);
    }
    else if (hz >= 1'000) {
        return std::format("{:.2f} kHz", hz / 1'000.0);
    } 
    else {
        return std::format("{} Hz", hz);
    }
}


enum class capturing : std::uint8_t{
    IDLE,
    CAPTURING,
    DONE
};

struct status_conf{
    bool dev_con;
    capturing cap_status_;
    std::string_view name_of_file = "capture.vcd";
    [[nodiscard]]std::string_view cap_status()const{
        switch (cap_status_) {
            case capturing::IDLE:return "IDLE";
            case capturing::CAPTURING:return "CAPTURING";
            case capturing::DONE:return "DONE";
            default:
            return "IDLE";
        }
    }
};