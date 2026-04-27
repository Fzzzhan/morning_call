#include "PreferenceManager.h"
#include "../utils/Logger.h"

PreferenceManager::PreferenceManager() {
    // Load profile on initialization
    auto profileOpt = repository_.loadProfile();
    if (profileOpt) {
        currentProfile_ = *profileOpt;
        LOG_INFO("User profile loaded successfully");
    } else {
        // Use defaults if no profile exists
        currentProfile_ = UserProfile();
        LOG_INFO("Using default user profile");
    }
}

UserProfile PreferenceManager::loadProfile() {
    auto profileOpt = repository_.loadProfile();
    if (profileOpt) {
        currentProfile_ = *profileOpt;
        return currentProfile_;
    }
    return UserProfile();  // Return defaults
}

bool PreferenceManager::saveProfile(const UserProfile& profile) {
    if (repository_.saveProfile(profile)) {
        currentProfile_ = profile;
        LOG_INFO("User profile saved successfully");
        return true;
    }
    LOG_ERROR("Failed to save user profile");
    return false;
}

void PreferenceManager::recordFavorite(const std::string& category) {
    currentProfile_.adjustCategoryPreference(category, 0.3f);
    saveProfile(currentProfile_);
    LOG_DEBUG("Recorded favorite for category: {}", category);
}

void PreferenceManager::recordSkip(const std::string& category) {
    currentProfile_.adjustCategoryPreference(category, -0.1f);
    saveProfile(currentProfile_);
    LOG_DEBUG("Recorded skip for category: {}", category);
}

void PreferenceManager::recordDislike(const std::string& category) {
    currentProfile_.adjustCategoryPreference(category, -0.5f);
    saveProfile(currentProfile_);
    LOG_DEBUG("Recorded dislike for category: {}", category);
}

bool PreferenceManager::resetPreferences() {
    if (repository_.resetAllPreferences()) {
        currentProfile_ = UserProfile();  // Reset to defaults
        LOG_INFO("User preferences reset to defaults");
        return true;
    }
    LOG_ERROR("Failed to reset user preferences");
    return false;
}
