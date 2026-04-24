#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <optional>

using json = nlohmann::json;

class JsonHelper {
public:
    // Load JSON from file
    static std::optional<json> loadFromFile(const std::string& filePath);

    // Save JSON to file
    static bool saveToFile(const std::string& filePath, const json& data);

    // Parse JSON string
    static std::optional<json> parse(const std::string& jsonString);

    // Get string value safely
    static std::string getString(const json& j, const std::string& key, const std::string& defaultValue = "");

    // Get int value safely
    static int getInt(const json& j, const std::string& key, int defaultValue = 0);

    // Get float value safely
    static float getFloat(const json& j, const std::string& key, float defaultValue = 0.0f);

    // Get bool value safely
    static bool getBool(const json& j, const std::string& key, bool defaultValue = false);
};
