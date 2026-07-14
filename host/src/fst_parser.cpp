#include <fcntl.h>
#include <iostream>
#include <cmath>
#include "fst_parser.hpp"
#include <cstdint>
#include <format>
#include <fstream>
#include <span>
#include <string>
#include <chrono>
#include <sys/types.h>
extern "C" {
#include "fstapi.h"
}

void fst_parse(std::span<const uint8_t> to_parse, const logic_an_input &config, const std::string &name_of_file) {
    auto *fst_object = fstWriterCreate(name_of_file.c_str(), 1);
    fstWriterSetTimescale(fst_object, -15);
    fstWriterSetScope(fst_object, FST_ST_VCD_MODULE, "GPIO", nullptr);
    auto id = fstWriterCreateVar(fst_object, FST_VT_VCD_WIRE, FST_VD_INPUT, 1,
                                 std::string("gpio" + std::to_string(config.channel)).c_str(), 0);
    uint64_t current_time_fs = 0;
    fstWriterSetUpscope(fst_object);
    fstWriterEmitTimeChange(fst_object, 0);
    uint8_t status_of_bit{255};
    for (auto v : to_parse) {
        if (v != status_of_bit) {
            fstWriterEmitTimeChange(fst_object, current_time_fs);
            if (v == 0)
                fstWriterEmitValueChange(fst_object, id, "0");
            else
                fstWriterEmitValueChange(fst_object, id, "1");
            status_of_bit = v;
        }
        current_time_fs += step_fs;
    }
    fstWriterClose(fst_object);
}
