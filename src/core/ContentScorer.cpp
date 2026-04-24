#include "ContentScorer.h"
#include "../utils/Logger.h"
#include <algorithm>
#include <chrono>
#include <cmath>

ContentScorer::ContentScorer() = default;

float ContentScorer::calculateFreshnessWeight(const ContentItem& item) {
    auto now = std::chrono::system_clock::now();
    auto itemTime = item.getCreatedAt();

    auto duration = std::chrono::duration_cast<std::chrono::hours>(now - itemTime);
    int hoursSinceCreation = duration.count();

    // Freshness decays linearly over 30 days (720 hours)
    // New content (0-24h): weight 1.0
    // After 30 days: weight 0.5
    // After 60 days: weight 0.25
    const int maxHours = 720;  // 30 days

    if (hoursSinceCreation < 24) {
        return 1.0f;
    } else if (hoursSinceCreation > maxHours * 2) {
        return 0.25f;
    } else {
        float decay = 1.0f - (static_cast<float>(hoursSinceCreation) / (maxHours * 2));
        return std::max(0.25f, decay);
    }
}

float ContentScorer::calculatePreferenceWeight(const ContentItem& item,
                                                const UserProfile& profile,
                                                const std::vector<std::string>& skippedIds,
                                                const std::vector<std::string>& dislikedIds) {
    float weight = 1.0f;

    // Apply category preference from user profile
    std::string category = item.getCategoryString();
    float categoryPref = profile.getCategoryPreference(category);
    weight += categoryPref;  // Range: typically -1.0 to +1.0

    // Penalize if user skipped similar content
    bool wasSkipped = std::find(skippedIds.begin(), skippedIds.end(), item.getId()) != skippedIds.end();
    if (wasSkipped) {
        weight -= 0.1f;
    }

    // Strong penalty if user marked as not interested
    bool wasDisliked = std::find(dislikedIds.begin(), dislikedIds.end(), item.getId()) != dislikedIds.end();
    if (wasDisliked) {
        weight -= 0.5f;
    }

    // Clamp weight to reasonable range
    return std::max(0.1f, std::min(2.0f, weight));
}

float ContentScorer::scoreContent(const ContentItem& item,
                                   const UserProfile& profile,
                                   const std::vector<std::string>& shownIds,
                                   const std::vector<std::string>& skippedIds,
                                   const std::vector<std::string>& dislikedIds) {
    // Skip if already shown
    bool wasShown = std::find(shownIds.begin(), shownIds.end(), item.getId()) != shownIds.end();
    if (wasShown) {
        return 0.0f;  // Don't show again
    }

    // Calculate component scores
    float baseScore = item.getBaseScore();
    float freshnessWeight = calculateFreshnessWeight(item);
    float preferenceWeight = calculatePreferenceWeight(item, profile, skippedIds, dislikedIds);

    // Final score formula
    float finalScore = baseScore * freshnessWeight * preferenceWeight;

    return std::max(0.0f, std::min(1.0f, finalScore));
}

std::vector<ContentItem> ContentScorer::scoreAndSort(const std::vector<ContentItem>& items,
                                                       const UserProfile& profile,
                                                       const std::vector<std::string>& shownIds,
                                                       const std::vector<std::string>& skippedIds,
                                                       const std::vector<std::string>& dislikedIds) {
    std::vector<ContentItem> scoredItems;

    for (auto item : items) {
        float score = scoreContent(item, profile, shownIds, skippedIds, dislikedIds);

        if (score > 0.0f) {  // Only include items with positive scores
            item.setScore(score);
            scoredItems.push_back(item);
        }
    }

    // Sort by score descending
    std::sort(scoredItems.begin(), scoredItems.end(),
              [](const ContentItem& a, const ContentItem& b) {
                  return a.getScore() > b.getScore();
              });

    LOG_DEBUG("Scored and sorted {} items", scoredItems.size());

    return scoredItems;
}
