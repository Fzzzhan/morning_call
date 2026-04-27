#include "RSSFeedConfig.h"
#include "../utils/Logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <QStandardPaths>
#include <QString>

RSSFeedConfig::RSSFeedConfig() {
}

bool RSSFeedConfig::loadFromFile(const std::string& configPath) {
    feedUrls_.clear();

    std::ifstream file(configPath);
    if (!file.is_open()) {
        LOG_INFO("RSS config file not found: {}", configPath);
        return false;
    }

    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        std::string url = parseLine(line);
        if (!url.empty()) {
            feedUrls_.push_back(url);
            LOG_INFO("Added RSS feed [line {}]: {}", lineNum, url);
        }
    }

    file.close();

    if (feedUrls_.empty()) {
        LOG_INFO("No RSS feeds configured in: {}", configPath);
        return false;
    }

    LOG_INFO("Loaded {} RSS feed(s) from config", feedUrls_.size());
    return true;
}

std::string RSSFeedConfig::parseLine(const std::string& line) {
    // Trim whitespace
    std::string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
    trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);

    // Skip empty lines
    if (trimmed.empty()) {
        return "";
    }

    // Skip comment lines
    if (trimmed[0] == '#') {
        return "";
    }

    // Basic URL validation
    if (trimmed.find("http://") != 0 && trimmed.find("https://") != 0) {
        LOG_WARN("Invalid RSS URL (must start with http:// or https://): {}", trimmed);
        return "";
    }

    return trimmed;
}

std::string RSSFeedConfig::getDefaultConfigPath() {
    // Try resources/rss_feeds.txt first (for development)
    std::string localPath = "resources/rss_feeds.txt";
    std::ifstream testFile(localPath);
    if (testFile.good()) {
        testFile.close();
        return localPath;
    }

    // Fall back to user config directory
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString configPath = configDir + "/rss_feeds.txt";
    return configPath.toStdString();
}
