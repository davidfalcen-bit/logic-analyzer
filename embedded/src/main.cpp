#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <hardware/gpio.h>
#include <hardware/structs/clocks.h>
#include <hardware/structs/io_bank0.h>
#include <iostream>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>
#include "config.hpp"
#include "parser.hpp"
#include "sampler.hpp"
Sampler smp;


int main() {
    stdio_init_all();
    sleep_ms(2000);
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }

    auto slice = pwm_gpio_to_slice_num(15);
    auto config = pwm_get_default_config();
    auto sys_clck = clock_get_hz(clk_sys);
    auto wrap_val = (sys_clck / 100000) -1;
    auto level_val = (wrap_val+1)/2;
    pwm_config_set_wrap(&config, wrap_val);
    gpio_set_function(15, GPIO_FUNC_PWM);
    pwm_init(slice, &config, true);
    pwm_set_gpio_level(15, level_val);

    
    auto result = parse_cin();
    std::cout << +result.channel << '\n' << result.hz << '\n' << result.samples << '\n';

    smp.init(result);
    smp.start_sampling();
    auto x = smp.samples();
    if(!x){
        std::cout << "We sucked";
        abort();
    }
    for(std::uint32_t i{}; i < x.value().size(); i++){
        std::cout << i+1 << ':' << +x.value()[i] << '\n';
    }
    std::cout << std::flush;
    sleep_ms(200);
    abort();
}