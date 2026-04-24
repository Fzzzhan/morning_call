#include "SeedContentLoader.h"
#include "../utils/JsonHelper.h"
#include "../utils/Logger.h"
#include <algorithm>
#include <random>
#include <filesystem>

SeedContentLoader::SeedContentLoader() = default;

std::string SeedContentLoader::getDefaultSeedPath() {
    // Check relative to executable
    std::string paths[] = {
        "resources/seed_content.json",
        "../resources/seed_content.json",
        "../../resources/seed_content.json"
    };

    for (const auto& path : paths) {
        if (std::filesystem::exists(path)) {
            return path;
        }
    }

    LOG_WARN("Default seed content file not found, using fallback path");
    return "resources/seed_content.json";
}

bool SeedContentLoader::loadFromFile(const std::string& filePath) {
    content_.clear();

    auto jsonOpt = JsonHelper::loadFromFile(filePath);
    if (!jsonOpt) {
        LOG_ERROR("Failed to load seed content from: {}", filePath);
        return false;
    }

    auto& jsonData = *jsonOpt;
    if (!jsonData.is_array()) {
        LOG_ERROR("Seed content JSON must be an array");
        return false;
    }

    for (const auto& item : jsonData) {
        try {
            std::string id = JsonHelper::getString(item, "id");
            std::string title = JsonHelper::getString(item, "title");
            std::string categoryStr = JsonHelper::getString(item, "category");
            std::string summary = JsonHelper::getString(item, "summary");
            std::string whyImportant = JsonHelper::getString(item, "whyImportant");
            std::string actionSuggestion = JsonHelper::getString(item, "actionSuggestion");
            std::string source = JsonHelper::getString(item, "source", "内置");
            float baseScore = JsonHelper::getFloat(item, "baseScore", 0.5f);

            ContentCategory category = ContentItem::categoryFromString(categoryStr);

            ContentItem contentItem(id, title, category, summary, whyImportant,
                                    actionSuggestion, source, baseScore);

            content_.push_back(contentItem);
        } catch (const std::exception& e) {
            LOG_WARN("Failed to parse content item: {}", e.what());
            continue;
        }
    }

    LOG_INFO("Loaded {} content items from seed file", content_.size());
    return !content_.empty();
}

std::vector<ContentItem> SeedContentLoader::getAllContent() const {
    return content_;
}

std::vector<ContentItem> SeedContentLoader::getContentByCategory(ContentCategory category) const {
    std::vector<ContentItem> filtered;
    for (const auto& item : content_) {
        if (item.getCategory() == category) {
            filtered.push_back(item);
        }
    }
    return filtered;
}

std::vector<ContentItem> SeedContentLoader::getRandomContent(size_t count) const {
    if (content_.empty()) {
        return {};
    }

    std::vector<ContentItem> result = content_;

    // Shuffle using random device
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(result.begin(), result.end(), gen);

    // Return up to count items
    if (result.size() > count) {
        result.resize(count);
    }

    return result;
}
