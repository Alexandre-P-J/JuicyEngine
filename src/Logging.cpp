#include <Logging.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace JuicyEngine {

void Logging::init_loggers() {
    static bool first_exec = true;
    if (first_exec) {
        first_exec = false;
        spdlog::stdout_color_mt("ENGINE");
        spdlog::stdout_color_mt("IO");
        spdlog::stdout_color_mt("LUA");
        spdlog::stdout_color_mt("TESTING");
        spdlog::set_pattern("[%H:%M:%S] [%^%n%$] %v");
    }
}

}  // namespace JuicyEngine
