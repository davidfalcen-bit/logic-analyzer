#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <hardware/gpio.h>
#include <hardware/structs/clocks.h>
#include <hardware/structs/io_bank0.h>
#include <iostream>
#include <pico/error.h>
#include <pico/platform/common.h>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>
#include <span>
#include <unistd.h>
#include "config.hpp"
#include "sampler.hpp"
#include "hardware/vreg.h"
Sampler smp;
int main() {
    vreg_set_voltage(VREG_VOLTAGE_1_15);
    sleep_ms(150);
    set_sys_clock_hz(200'000'000, true);
    stdio_init_all();
    sleep_ms(500);
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }
    stdio_set_translate_crlf(&stdio_usb, false);
    // printf("actual clk_sys: %lu Hz\n", clock_get_hz(clk_sys));
    for (;;) {
        logic_an_input result{};
        while (true) {
            int fsymbol = getchar_timeout_us(0);
            if (fsymbol == PICO_ERROR_TIMEOUT) {
                tight_loop_contents();
            } else if (fsymbol == ping::PING_HANDSHAKE) {
                putchar(ping::PONG_HANDSHAKE);
                fflush(stdout);
            } else if (fsymbol == ping::SIGN_OF_STRUCT) {
                auto *ptr = reinterpret_cast<uint8_t *>(&result);
                *ptr = fsymbol;
                size_t total_for_reading = sizeof(logic_an_input) - 1;
                size_t already_read{};
                while (total_for_reading > already_read) {
                    fsymbol = getchar_timeout_us(2500);
                    if (fsymbol != PICO_ERROR_TIMEOUT) {
                        *(ptr + (++already_read)) = static_cast<uint8_t>(fsymbol);
                    } else {
                        break;
                    }
                }
                if (result.samples < 200'001 && result.hz < 200'000'001 && result.msg == 6 && result.amm < 8) {
                    putchar(ping::READY_CONFIG);
                    fflush(stdout);
                    break;
                }
                std::memset(&result, 0, sizeof(logic_an_input));
            }
        }
        sleep_ms(2);
        smp.init(result);
        smp.start_sampling();
        auto x = smp.samples();

        while (true) {
            int fsymbol = getchar_timeout_us(0);
            if (fsymbol == ping::PING_SAMPLING) {
                putchar(ping::PONG_SAMPLING);
                fflush(stdout);
                auto time_now = get_absolute_time();
                while (absolute_time_diff_us(time_now, get_absolute_time()) <= 600'000) {
                    fsymbol = getchar_timeout_us(0);
                    if (fsymbol == ping::PING_SEND_SAMPLING)
                        break;
                    else if (fsymbol == ping::PING_SAMPLING) {
                        putchar(ping::PONG_SAMPLING);
                        fflush(stdout);
                        time_now = get_absolute_time();
                    }
                }
                if (fsymbol != ping::PING_SEND_SAMPLING)
                    break;
                fwrite(smp.samples().value().data(), sizeof(smp.samples().value()[0]), smp.samples().value().size(),
                       stdout);
                fflush(stdout);
                break;
            }
            tight_loop_contents();
        }
    }
}