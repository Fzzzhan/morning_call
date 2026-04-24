#pragma once

#include "../models/UserProfile.h"
#include <optional>
#include <string>

class UserProfileRepository {
public:
    UserProfileRepository() = default;

    // Load and save user profile
    std::optional<UserProfile> loadProfile();
    bool saveProfile(const UserProfile& profile);

    // Individual preference operations
    bool savePreference(const std::string& key, const std::string& value);
    std::optional<std::string> getPreference(const std::string& key);

    // Reset all preferences
    bool resetAllPreferences();
};
