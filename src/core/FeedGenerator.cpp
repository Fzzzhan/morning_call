#include "FeedGenerator.h"
#include "../services/RSSFeedConfig.h"
#include "../utils/Logger.h"
#include <algorithm>

FeedGenerator::FeedGenerator() = default;

bool FeedGenerator::initialize(const std::string& seedPath) {
    LOG_INFO("Initializing FeedGenerator with seed file: {}", seedPath);

    if (!seedLoader_.loadFromFile(seedPath)) {
        LOG_ERROR("Failed to load seed content");
        return false;
    }

    // Load seed content into database if not already there
    auto allContent = seedLoader_.getAllContent();
    if (!repository_.saveContents(allContent)) {
        LOG_WARN("Some seed content may not have been saved to database");
    }

    LOG_INFO("FeedGenerator initialized with {} items", allContent.size());
    return true;
}

bool FeedGenerator::initializeWithProviders(ContentProviderManager& providerManager) {
    LOG_INFO("Initializing FeedGenerator with ContentProviderManager");

    // Fetch content from all providers
    auto allContent = providerManager.fetchAllContent();

    if (allContent.empty()) {
        LOG_ERROR("No content fetched from providers");
        return false;
    }

    // Save to database
    if (!repository_.saveContents(allContent)) {
        LOG_WARN("Some content may not have been saved to database");
    }

    LOG_INFO("FeedGenerator initialized with {} items from providers", allContent.size());
    return true;
}

std::vector<ContentItem> FeedGenerator::selectBestFromCategory(
    ContentCategory category, size_t count,
    const std::vector<ContentItem>& scoredItems) {

    std::vector<ContentItem> categoryItems;

    // Filter by category
    for (const auto& item : scoredItems) {
        if (item.getCategory() == category) {
            categoryItems.push_back(item);
        }
    }

    // Return top N items
    if (categoryItems.size() > count) {
        categoryItems.resize(count);
    }

    return categoryItems;
}

DailyFeed FeedGenerator::generateDailyFeed(const UserProfile& profile) {
    return generateCustomFeed(
        profile.getThinkingCount(),
        profile.getHabitCount(),
        profile.getKnowledgeCount(),
        profile.getTrendCount(),
        profile
    );
}

DailyFeed FeedGenerator::generateCustomFeed(int thinkingCount, int habitCount,
                                             int knowledgeCount, int trendCount,
                                             const UserProfile& profile) {
    LOG_INFO("Generating feed with distribution: T:{}, H:{}, K:{}, Tr:{}",
             thinkingCount, habitCount, knowledgeCount, trendCount);

    // Get all available content
    std::vector<ContentItem> allContent = repository_.getAllContents();

    // Get user history
    auto shownIds = repository_.getShownContentIds();
    auto skippedIds = repository_.getActionsByType("skip");
    auto dislikedIds = repository_.getActionsByType("dislike");

    // Score and sort all content
    auto scoredItems = scorer_.scoreAndSort(allContent, profile, shownIds, skippedIds, dislikedIds);

    // Select items by category
    std::vector<ContentItem> selectedItems;

    auto thinkingItems = selectBestFromCategory(ContentCategory::Thinking, thinkingCount, scoredItems);
    selectedItems.insert(selectedItems.end(), thinkingItems.begin(), thinkingItems.end());

    auto habitItems = selectBestFromCategory(ContentCategory::Habit, habitCount, scoredItems);
    selectedItems.insert(selectedItems.end(), habitItems.begin(), habitItems.end());

    auto knowledgeItems = selectBestFromCategory(ContentCategory::Knowledge, knowledgeCount, scoredItems);
    selectedItems.insert(selectedItems.end(), knowledgeItems.begin(), knowledgeItems.end());

    auto trendItems = selectBestFromCategory(ContentCategory::Trend, trendCount, scoredItems);
    selectedItems.insert(selectedItems.end(), trendItems.begin(), trendItems.end());

    // Create daily feed
    std::string today = repository_.getTodayDate();
    DailyFeed feed(today, selectedItems);

    // Save to database
    if (repository_.saveDailyFeed(feed)) {
        LOG_INFO("Generated and saved daily feed with {} items", selectedItems.size());
    } else {
        LOG_ERROR("Failed to save daily feed to database");
    }

    return feed;
}
