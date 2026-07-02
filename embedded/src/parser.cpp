#include "parser.hpp"
#include "iostream"
#include <array>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <span>
#include <string>
#include <string_view>
#include <system_error>


namespace aur{
    bool getline(std::span<char> buffer){
        if(buffer.size() == 0) return false;
        for(unsigned int i{}; i < buffer.size()-1; i++){
            char ch;
            if(!std::cin.get(ch)){
                return false;
            }
            if(ch == '\r' || ch == '\n'){
                std::cout << "\r\n";
                buffer[i] = '\0';
                return true;
            }
            buffer[i] = ch;
        }
        buffer[0] = '\0';
        std::cout << "\r\n";
        return false;
    }
}

static bool multiplication_will_overflow(uint32_t a, uint32_t b) {
    return b != 0 && a > std::numeric_limits<uint32_t>::max() / b;
}

static uint32_t from_string_to_hz(std::string_view input) {
    uint32_t digit{};
    auto [ptr, er]{std::from_chars(input.begin(), input.end(), digit)};
    if(er != std::errc()) return 0;
    input.remove_prefix(ptr - input.data());
    while (!input.empty()&& isspace(input.front())) {
        input.remove_prefix(1);
    }
    if (input.size() == 3 && tolower(input[0]) == 'm' && tolower(input[1]) == 'h' && tolower(input[2]) == 'z') {
        if (multiplication_will_overflow(digit, 1000000) || (digit * 1000000 > 200000000))
            return 0;
        else
            return digit * 1000000;
    }
    if (input.size() == 3  && tolower(input[0]) == 'k' && tolower(input[1]) == 'h' && tolower(input[2]) == 'z') {
        if (multiplication_will_overflow(digit, 1000) || digit * 1000 > 200000000)
            return 0;
        else
            return digit * 1000;
    }
    
    if (input.size() == 2  && tolower(input[0]) == 'h' && tolower(input[1]) == 'z') {
        if (digit > 200000000)
            return 0;
        else
            return digit;
    }
    return 0;
}

logic_an_input parse_cin() {
    state_of_parse st = state_of_parse::PARSING_CHANNEL;
    std::string_view input;
    std::array<char, 16> buffer{};
    logic_an_input result{};
    while (true) {
        switch (st) {
        case state_of_parse::PARSING_CHANNEL: {
            std::cout << "What channel do you want to use?\r\n";
            if(!aur::getline(buffer)){
                std::cout << "Your input is wrong, try again\r\n";
                break;
            }
            input = std::string_view{buffer.data()};
            auto [ptr, error] = std::from_chars(input.begin(), input.end(), result.channel);
            if (error == std::errc() && ptr == input.end()) {
                if (result.channel > 28 ) {
                    std::cout << "Channels are from 0 to 28. Try again please.\r\n";
                    continue;
                }
                st = state_of_parse::PARSING_HZ;
                break;
            }
            std::cout << "Your input is wrong, try again\r\n";
            break;
        }
        case state_of_parse::PARSING_HZ:
            std::cout << "Enter the sampling frequency.\r\n"
                      << "Format: <number> <unit>\r\n"
                      << "Supported units: Hz, kHz, MHz\r\n"
                      << "Examples: 100 Hz, 13 khz, 20MHz\r\n"
                      << "Allowed hz from 1Hz to 200MHz\r\n";
            if(!aur::getline(buffer)){
                std::cout << "Your input is wrong, try again\r\n";
                break;
            }
            input = std::string_view{buffer.data()};
            result.hz = from_string_to_hz(input);
            if (result.hz == 0) {
                std::cout
                    << "Your input contains an error.\r\nIt can be not correct sufix or to big num, please try again\r\n";
                continue;
            }
            st = state_of_parse::PARSING_SAMPLES;
            break;
        case state_of_parse::PARSING_SAMPLES:
            std::cout << "How much samples do you want to use?\r\nMaximum is: 1'000'000\r\n";
            if(!aur::getline(buffer)){
                std::cout << "Your input is wrong, try again\r\n";
                break;
            }
            input = std::string_view{buffer.data()};
            auto [ptr, error] = std::from_chars(input.begin(), input.end(), result.samples);
            if (error == std::errc() && ptr == input.end()) {
                if (result.samples == 0 || result.samples > 1000000) {
                    std::cout << "Your input contains an error.\r\nIt can not be a negative or zero\r\n";
                    continue;
                }
                return result;
            }
            std::cout << "Your input is wrong, try again\r\n";
            break;
        }
        if (std::cin.eof()) {
            std::cout << "You pressed unavaliable key-bind for this CLI, by default we use channel 15, Freq: 100 Mhz, and 100,000 samples, sorry for inconvient behaviour\r\n";
            result = {.channel = 15, .hz = 100000000, .samples = 100000};
            return result;
        }
    }
}
