#include "config.hpp"
#include <array>
#include <cstdint>
#include <expected>
#include <optional>
#include <span>

class Sampler{
public:
    Sampler() = default; 
    void init(const logic_an_input inpt);
    void start_sampling();
    [[nodiscard]]std::optional<std::span<const std::uint8_t>>samples() const;
private:
    bool still_measuring{true};
    static constexpr std::uint32_t max_samples = 100000;
    logic_an_input inpt_for_sampling;
    int64_t tact_time;
    std::array<uint8_t, max_samples>samples_{};
};