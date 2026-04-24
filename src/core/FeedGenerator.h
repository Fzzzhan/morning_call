#pragma once

#include "../models/ContentItem.h"
#include "../models/UserProfile.h"
#include "../models/DailyFeed.h"
#include "../data/ContentRepository.h"
#include "../services/SeedContentLoader.h"
#include "../services/ContentProvider.h"
#include "ContentScorer.h"
#include <memory>
#include <vector>

class FeedGenerator {
public:
    FeedGenerator();

    // Initialize with seed content and repositories
    bool initialize(const std::string& seedPath);

    // Initialize with multiple content providers
    bool initializeWithProviders(ContentProviderManager& providerManager);

    // Generate daily feed based on user profile
    DailyFeed generateDailyFeed(const UserProfile& profile);

    // Generate feed with custom distribution
    DailyFeed generateCustomFeed(int thinkingCount, int habitCount,
                                 int knowledgeCount, int trendCount,
                                 const UserProfile& profile);

    // Check if generator is ready
    bool isReady() const { return seedLoader_.isLoaded(); }

private:
    // Select N best items from a category
    std::vector<ContentItem> selectBestFromCategory(
        ContentCategory category, size_t count,
        const std::vector<ContentItem>& scoredItems);

    SeedContentLoader seedLoader_;
    ContentScorer scorer_;
    ContentRepository repository_;
};
