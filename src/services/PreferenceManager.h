#pragma once

#include "../models/UserProfile.h"
#include "../data/UserProfileRepository.h"
#include <memory>

class PreferenceManager {
public:
    PreferenceManager();

    // Load user profile from database
    UserProfile loadProfile();

    // Save user profile to database
    bool saveProfile(const UserProfile& profile);

    // Record user action and update preferences
    void recordFavorite(const std::string& category);
    void recordSkip(const std::string& category);
    void recordDislike(const std::string& category);

    // Reset all preferences to defaults
    bool resetPreferences();

private:
    UserProfileRepository repository_;
    UserProfile currentProfile_;
};
