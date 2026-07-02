#include "sampler.hpp"
#include "config.hpp"
#include <cstdint>
#include <hardware/gpio.h>
#include <optional>
#include <pico/platform/common.h>
#include <pico/time.h>
#include <span>

void Sampler::init(const logic_an_input inpt){
    inpt_for_sampling = inpt;
    tact_time = 1'000'00 / inpt_for_sampling.hz;
    gpio_init(inpt_for_sampling.channel);
    gpio_set_dir(inpt_for_sampling.channel, GPIO_IN);
}

[[nodiscard]] std::optional<std::span<const std::uint8_t>>Sampler::samples()const{
    if(still_measuring){
        return std::nullopt;
    }
    std::span<const uint8_t> packet = samples_;
    return packet.first(inpt_for_sampling.samples);
}

void Sampler::start_sampling(){
    if (tact_time <= 1) {
        return;
    }
    for(uint32_t i{}; i < inpt_for_sampling.samples; i++){
        auto time_than = get_absolute_time();
        while (true) {
            if (absolute_time_diff_us(time_than, get_absolute_time()) >= tact_time) {
                samples_[i] = gpio_get(inpt_for_sampling.channel);
                break;
            }
            tight_loop_contents();
        }
    }
}