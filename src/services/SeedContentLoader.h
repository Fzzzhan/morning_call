#pragma once

#include "../models/ContentItem.h"
#include <vector>
#include <string>

class SeedContentLoader {
public:
    SeedContentLoader();

    // Load seed content from JSON file
    bool loadFromFile(const std::string& filePath);

    // Get all loaded content
    std::vector<ContentItem> getAllContent() const;

    // Get content by category
    std::vector<ContentItem> getContentByCategory(ContentCategory category) const;

    // Get random content items
    std::vector<ContentItem> getRandomContent(size_t count) const;

    // Check if content is loaded
    bool isLoaded() const { return !content_.empty(); }

    // Get default seed content file path
    static std::string getDefaultSeedPath();

private:
    std::vector<ContentItem> content_;
};
