#include "JsonHelper.h"
#include "Logger.h"
#include <fstream>
#include <iostream>

std::optional<json> JsonHelper::loadFromFile(const std::string& filePath) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            LOG_ERROR("Failed to open JSON file: {}", filePath);
            return std::nullopt;
        }

        json j;
        file >> j;
        return j;
    } catch (const json::exception& e) {
        LOG_ERROR("JSON parse error in file {}: {}", filePath, e.what());
        return std::nullopt;
    } catch (const std::exception& e) {
        LOG_ERROR("Error loading JSON file {}: {}", filePath, e.what());
        return std::nullopt;
    }
}

bool JsonHelper::saveToFile(const std::string& filePath, const json& data) {
    try {
        std::ofstream file(filePath);
        if (!file.is_open()) {
            LOG_ERROR("Failed to open file for writing: {}", filePath);
            return false;
        }

        file << data.dump(2);  // Pretty print with 2 spaces
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Error saving JSON file {}: {}", filePath, e.what());
        return false;
    }
}

std::optional<json> JsonHelper::parse(const std::string& jsonString) {
    try {
        return json::parse(jsonString);
    } catch (const json::exception& e) {
        LOG_ERROR("JSON parse error: {}", e.what());
        return std::nullopt;
    }
}

std::string JsonHelper::getString(const json& j, const std::string& key, const std::string& defaultValue) {
    try {
        if (j.contains(key) && j[key].is_string()) {
            return j[key].get<std::string>();
        }
    } catch (...) {
        // Fall through to default
    }
    return defaultValue;
}

int JsonHelper::getInt(const json& j, const std::string& key, int defaultValue) {
    try {
        if (j.contains(key) && j[key].is_number_integer()) {
            return j[key].get<int>();
        }
    } catch (...) {
        // Fall through to default
    }
    return defaultValue;
}

float JsonHelper::getFloat(const json& j, const std::string& key, float defaultValue) {
    try {
        if (j.contains(key) && j[key].is_number()) {
            return j[key].get<float>();
        }
    } catch (...) {
        // Fall through to default
    }
    return defaultValue;
}

bool JsonHelper::getBool(const json& j, const std::string& key, bool defaultValue) {
    try {
        if (j.contains(key) && j[key].is_boolean()) {
            return j[key].get<bool>();
        }
    } catch (...) {
        // Fall through to default
    }
    return defaultValue;
}
