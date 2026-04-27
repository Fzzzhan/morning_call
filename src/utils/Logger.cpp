#include "Logger.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>
#include <cstdlib>

std::shared_ptr<spdlog::logger> Logger::logger_ = nullptr;

std::string Logger::getLogPath() {
    const char* home = std::getenv("HOME");
    if (!home) {
        return "/tmp/morning_call.log";
    }

    std::string logDir = std::string(home) + "/.local/share/morning_call/logs";
    std::filesystem::create_directories(logDir);

    return logDir + "/morning_call.log";
}

void Logger::initialize() {
    if (logger_) {
        return;  // Already initialized
    }

    try {
        std::string logPath = getLogPath();

        // Create sinks: file and console
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath, true);
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        // Set format pattern
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

        // Create logger with both sinks
        std::vector<spdlog::sink_ptr> sinks{file_sink, console_sink};
        logger_ = std::make_shared<spdlog::logger>("morning_call", sinks.begin(), sinks.end());

        // Set log level
        logger_->set_level(spdlog::level::debug);
        logger_->flush_on(spdlog::level::info);

        // Register logger
        spdlog::register_logger(logger_);

        logger_->info("Logger initialized successfully");
    } catch (const spdlog::spdlog_ex& ex) {
        // Fallback to console-only logger
        logger_ = spdlog::stdout_color_mt("morning_call");
        logger_->error("Log file initialization failed: {}", ex.what());
    }
}

std::shared_ptr<spdlog::logger> Logger::get() {
    if (!logger_) {
        initialize();
    }
    return logger_;
}
