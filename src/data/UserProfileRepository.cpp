#include "UserProfileRepository.h"
#include "Database.h"
#include <iostream>
#include <sstream>

std::optional<UserProfile> UserProfileRepository::loadProfile() {
    auto& db = Database::getInstance();
    if (!db.isOpen()) return std::nullopt;

    UserProfile profile;

    // Load integer counts
    auto thinking = getPreference("thinking_count");
    if (thinking) profile.setThinkingCount(std::stoi(*thinking));

    auto habit = getPreference("habit_count");
    if (habit) profile.setHabitCount(std::stoi(*habit));

    auto knowledge = getPreference("knowledge_count");
    if (knowledge) profile.setKnowledgeCount(std::stoi(*knowledge));

    auto trend = getPreference("trend_count");
    if (trend) profile.setTrendCount(std::stoi(*trend));

    // Load string preferences
    auto refreshTime = getPreference("refresh_time");
    if (refreshTime) profile.setRefreshTime(*refreshTime);

    auto themeMode = getPreference("theme_mode");
    if (themeMode) profile.setThemeMode(*themeMode);

    // Load boolean preferences
    auto autoStart = getPreference("auto_start");
    if (autoStart) profile.setAutoStart(*autoStart == "true");

    auto minimizeToTray = getPreference("minimize_to_tray");
    if (minimizeToTray) profile.setMinimizeToTray(*minimizeToTray == "true");

    // Load category preferences
    for (const auto& category : {"Thinking", "Habit", "Knowledge", "Trend"}) {
        std::string key = "category_pref_" + std::string(category);
        auto pref = getPreference(key);
        if (pref) {
            profile.adjustCategoryPreference(category, std::stof(*pref));
        }
    }

    return profile;
}

bool UserProfileRepository::saveProfile(const UserProfile& profile) {
    auto& db = Database::getInstance();
    if (!db.isOpen()) return false;

    db.beginTransaction();

    bool success = true;
    success &= savePreference("thinking_count", std::to_string(profile.getThinkingCount()));
    success &= savePreference("habit_count", std::to_string(profile.getHabitCount()));
    success &= savePreference("knowledge_count", std::to_string(profile.getKnowledgeCount()));
    success &= savePreference("trend_count", std::to_string(profile.getTrendCount()));
    success &= savePreference("refresh_time", profile.getRefreshTime());
    success &= savePreference("theme_mode", profile.getThemeMode());
    success &= savePreference("auto_start", profile.getAutoStart() ? "true" : "false");
    success &= savePreference("minimize_to_tray", profile.getMinimizeToTray() ? "true" : "false");

    // Save category preferences
    for (const auto& category : {"Thinking", "Habit", "Knowledge", "Trend"}) {
        std::string key = "category_pref_" + std::string(category);
        float pref = profile.getCategoryPreference(category);
        success &= savePreference(key, std::to_string(pref));
    }

    if (success) {
        return db.commit();
    } else {
        db.rollback();
        return false;
    }
}

bool UserProfileRepository::savePreference(const std::string& key, const std::string& value) {
    auto& db = Database::getInstance();
    if (!db.isOpen()) return false;

    const char* sql = "INSERT OR REPLACE INTO user_preferences (key, value) VALUES (?, ?)";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

std::optional<std::string> UserProfileRepository::getPreference(const std::string& key) {
    auto& db = Database::getInstance();
    if (!db.isOpen()) return std::nullopt;

    const char* sql = "SELECT value FROM user_preferences WHERE key = ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getHandle(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }

    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<std::string> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return result;
}

bool UserProfileRepository::resetAllPreferences() {
    auto& db = Database::getInstance();
    if (!db.isOpen()) return false;

    const char* sql = "DELETE FROM user_preferences";
    return db.execute(sql);
}
