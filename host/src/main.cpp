#include <cstddef>
#include <cstdio>
#include <format>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/ftxui.hpp>
#include <ftxui/screen/screen.hpp>
#include <string>
#include <string_view>
#include <utility>
#include "../../embedded/include/config.hpp"
#include <locale>
#include "elements_of_ui.hpp"

int main(){
    std::locale::global(std::locale("en_US.UTF-8"));
    logic_an_input inpt{.channel = 3, .hz = 1'000'000, .samples = 10000};
    status_conf stats{};
    std::string channel_placeholder;
    std::string frequency_placeholder;
    std::string samples_placeholder;

    auto channel_inp = ftxui::Input(&channel_placeholder, "");
    auto freq_inp = ftxui::Input(&frequency_placeholder, "");
    auto sample_inp = ftxui::Input(&samples_placeholder, "");
    auto inputs = ftxui::Container::Vertical({
        channel_inp,
        freq_inp,
        sample_inp,
    });

    auto renderer = ftxui::Renderer(inputs, [&] {
        return make_layout(inpt, stats, channel_inp, freq_inp, sample_inp);
    });
    auto screen = ftxui::ScreenInteractive::TerminalOutput();
    auto exit =screen.ExitLoopClosure();
    auto app = ftxui::CatchEvent(renderer, [&](ftxui::Event ev){
        if(ev == ftxui::Event::Character('q')){
            exit();
            return true;
        }
        else {
            return false;
        }
    });
    screen.Loop(app);
}
