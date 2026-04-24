#pragma once

#include <string>
#include <map>

class UserProfile {
public:
    UserProfile() = default;

    // Getters
    int getThinkingCount() const { return thinkingCount_; }
    int getHabitCount() const { return habitCount_; }
    int getKnowledgeCount() const { return knowledgeCount_; }
    int getTrendCount() const { return trendCount_; }

    std::string getRefreshTime() const { return refreshTime_; }
    bool getAutoStart() const { return autoStart_; }
    bool getMinimizeToTray() const { return minimizeToTray_; }
    std::string getThemeMode() const { return themeMode_; }

    // Category preferences (weighted scoring adjustments)
    float getCategoryPreference(const std::string& category) const;
    void adjustCategoryPreference(const std::string& category, float delta);

    // Setters
    void setThinkingCount(int count) { thinkingCount_ = count; }
    void setHabitCount(int count) { habitCount_ = count; }
    void setKnowledgeCount(int count) { knowledgeCount_ = count; }
    void setTrendCount(int count) { trendCount_ = count; }

    void setRefreshTime(const std::string& time) { refreshTime_ = time; }
    void setAutoStart(bool enable) { autoStart_ = enable; }
    void setMinimizeToTray(bool enable) { minimizeToTray_ = enable; }
    void setThemeMode(const std::string& mode) { themeMode_ = mode; }

    // Reset preferences
    void resetToDefaults();

private:
    // Content distribution (out of 10 items)
    int thinkingCount_{3};
    int habitCount_{2};
    int knowledgeCount_{3};
    int trendCount_{2};

    // UI preferences
    std::string refreshTime_{"08:00"};
    bool autoStart_{false};
    bool minimizeToTray_{true};
    std::string themeMode_{"light"};  // "light" or "dark"

    // Category scoring adjustments based on user behavior
    std::map<std::string, float> categoryPreferences_;
};
