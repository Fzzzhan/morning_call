#pragma once

#include "../models/ContentItem.h"
#include "../models/UserProfile.h"
#include <vector>
#include <string>

class ContentScorer {
public:
    ContentScorer();

    // Score a single content item based on user profile and history
    float scoreContent(const ContentItem& item, const UserProfile& profile,
                      const std::vector<std::string>& shownIds,
                      const std::vector<std::string>& skippedIds,
                      const std::vector<std::string>& dislikedIds);

    // Score multiple items and return them sorted by score (descending)
    std::vector<ContentItem> scoreAndSort(const std::vector<ContentItem>& items,
                                          const UserProfile& profile,
                                          const std::vector<std::string>& shownIds,
                                          const std::vector<std::string>& skippedIds,
                                          const std::vector<std::string>& dislikedIds);

private:
    // Calculate freshness weight based on age
    float calculateFreshnessWeight(const ContentItem& item);

    // Calculate preference weight based on user actions
    float calculatePreferenceWeight(const ContentItem& item, const UserProfile& profile,
                                    const std::vector<std::string>& skippedIds,
                                    const std::vector<std::string>& dislikedIds);
};
