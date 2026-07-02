#include "pico/stdlib.h"
#include <array>
#include <cstdint>
#include <iostream>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>
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
        std::cout << i << ':' << x.value()[i] << '\n';
    }
    
    abort();
}