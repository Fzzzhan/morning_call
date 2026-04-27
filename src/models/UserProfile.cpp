#include "UserProfile.h"

float UserProfile::getCategoryPreference(const std::string& category) const {
    auto it = categoryPreferences_.find(category);
    if (it != categoryPreferences_.end()) {
        return it->second;
    }
    return 0.0f;  // Neutral preference
}

void UserProfile::adjustCategoryPreference(const std::string& category, float delta) {
    categoryPreferences_[category] += delta;
    
    // Clamp to reasonable range [-1.0, 1.0]
    if (categoryPreferences_[category] > 1.0f) {
        categoryPreferences_[category] = 1.0f;
    } else if (categoryPreferences_[category] < -1.0f) {
        categoryPreferences_[category] = -1.0f;
    }
}

void UserProfile::resetToDefaults() {
    thinkingCount_ = 3;
    habitCount_ = 2;
    knowledgeCount_ = 3;
    trendCount_ = 2;
    refreshTime_ = "08:00";
    autoStart_ = false;
    minimizeToTray_ = true;
    themeMode_ = "light";
    categoryPreferences_.clear();
}
