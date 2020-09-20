#pragma once
#include <spdlog/spdlog.h>

namespace JuicyEngine {
enum class Level { critical, info, debug, warning, error };

class Logging {
    static void init_loggers();
    Logging() = default;

    template <class... Args>
    static void Generic(Level level, std::shared_ptr<spdlog::logger> logger,
                        Args const&... args) {
        switch (level) {
            case Level::info:
                logger->info(args...);
                break;
            case Level::critical:
                logger->critical(args...);
                break;
            case Level::debug:
                logger->debug(args...);
                break;
            case Level::warning:
                logger->warn(args...);
                break;
            case Level::error:
                logger->error(args...);
                break;
        }
    }

public:
    template <class... Args>
    static void Engine(Level level, Args const&... args) {
        init_loggers();
        Generic(level, spdlog::get("ENGINE"), args...);
    }

    template <class... Args>
    static void IO(Level level, Args const&... args) {
        init_loggers();
        Generic(level, spdlog::get("IO"), args...);
    }

    template <class... Args>
    static void Lua(Level level, Args const&... args) {
        init_loggers();
        Generic(level, spdlog::get("LUA"), args...);
    }

    template <class... Args>
    static void Testing(Level level, Args const&... args) {
        init_loggers();
        Generic(level, spdlog::get("TESTING"), args...);
    }
};

}  // namespace JuicyEngine
