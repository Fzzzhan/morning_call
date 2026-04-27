#pragma once

#include <vector>
#include <string>

// RSS Feed Configuration Loader
class RSSFeedConfig {
public:
    RSSFeedConfig();

    // Load RSS feed URLs from config file
    bool loadFromFile(const std::string& configPath);

    // Get all configured feed URLs
    std::vector<std::string> getFeedUrls() const { return feedUrls_; }

    // Check if RSS is enabled (has feeds)
    bool isEnabled() const { return !feedUrls_.empty(); }

    // Get default config file path
    static std::string getDefaultConfigPath();

private:
    std::vector<std::string> feedUrls_;

    // Parse single line from config file
    std::string parseLine(const std::string& line);
};
