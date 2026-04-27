#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <string>

class Logger {
public:
    static void initialize();
    static std::shared_ptr<spdlog::logger> get();

private:
    static std::shared_ptr<spdlog::logger> logger_;
    static std::string getLogPath();
};

// Convenience macros
#define LOG_INFO(...) Logger::get()->info(__VA_ARGS__)
#define LOG_WARN(...) Logger::get()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Logger::get()->error(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::get()->debug(__VA_ARGS__)
