#pragma once

#include "../models/ContentItem.h"
#include "../models/DailyFeed.h"
#include <vector>
#include <string>
#include <optional>

class ContentRepository {
public:
    ContentRepository() = default;

    // Content CRUD operations
    bool saveContent(const ContentItem& item);
    bool saveContents(const std::vector<ContentItem>& items);
    std::optional<ContentItem> getContentById(const std::string& id);
    std::vector<ContentItem> getAllContents();
    std::vector<ContentItem> getContentsByCategory(ContentCategory category);
    bool deleteContent(const std::string& id);

    // Daily feed operations
    bool saveDailyFeed(const DailyFeed& feed);
    std::optional<DailyFeed> getDailyFeed(const std::string& date);
    std::optional<DailyFeed> getTodaysFeed();
    std::vector<std::string> getShownContentIds();  // All content shown in any feed

    // User action tracking
    bool recordUserAction(const std::string& contentId, const std::string& actionType);
    std::vector<std::string> getActionsByType(const std::string& actionType);
    bool isContentInFavorites(const std::string& contentId);

    // Favorites management
    bool addToFavorites(const std::string& contentId);
    bool removeFromFavorites(const std::string& contentId);
    std::vector<ContentItem> getFavorites();

    // Utility
    std::string getTodayDate();  // Returns YYYY-MM-DD format
};
