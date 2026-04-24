#pragma once

#include "../models/ContentItem.h"
#include <vector>
#include <memory>

// Forward declarations
class QString;
class QDateTime;
struct RSSItem;

// Abstract interface for content providers
class IContentProvider {
public:
    virtual ~IContentProvider() = default;

    // Fetch content from this provider
    virtual std::vector<ContentItem> fetchContent() = 0;

    // Check if provider is available
    virtual bool isAvailable() = 0;

    // Get provider name for logging
    virtual std::string getProviderName() const = 0;
};

// Seed content provider (built-in JSON)
class SeedContentProvider : public IContentProvider {
public:
    explicit SeedContentProvider(const std::string& seedFilePath);

    std::vector<ContentItem> fetchContent() override;
    bool isAvailable() override;
    std::string getProviderName() const override { return "SeedContent"; }

private:
    std::string seedFilePath_;
    bool loaded_{false};
};

// Future: LLM-based content provider
// This is a placeholder for future implementation
class LLMContentProvider : public IContentProvider {
public:
    LLMContentProvider(const std::string& apiKey, const std::string& model);

    std::vector<ContentItem> fetchContent() override;
    bool isAvailable() override;
    std::string getProviderName() const override { return "LLM"; }

private:
    std::string apiKey_;
    std::string model_;
    bool configured_{false};

    // Future: Implement LLM API calls
    // - Generate daily content using Claude/GPT
    // - Classify and score content
    // - Extract action suggestions
};

// RSS content provider - fetches and parses RSS/Atom feeds
class RSSContentProvider : public IContentProvider {
public:
    explicit RSSContentProvider(const std::vector<std::string>& feedUrls);

    std::vector<ContentItem> fetchContent() override;
    bool isAvailable() override;
    std::string getProviderName() const override { return "RSS"; }

private:
    std::vector<std::string> feedUrls_;

    // Network operations
    QString fetchFromNetwork(const std::string& url);

    // Conversion and classification
    ContentItem convertToContentItem(const struct RSSItem& rssItem, const std::string& source);
    ContentCategory classifyContent(const struct RSSItem& rssItem);

    // Content generation helpers
    QString generateWhyImportant(const struct RSSItem& rssItem, ContentCategory category);
    QString generateActionSuggestion(const struct RSSItem& rssItem, ContentCategory category);
    float calculateFreshnessScore(const QDateTime& pubDate);
};

// Content provider manager - aggregates multiple providers
class ContentProviderManager {
public:
    ContentProviderManager();

    // Add a content provider
    void addProvider(std::shared_ptr<IContentProvider> provider);

    // Fetch content from all available providers
    std::vector<ContentItem> fetchAllContent();

    // Get provider count
    size_t getProviderCount() const { return providers_.size(); }

private:
    std::vector<std::shared_ptr<IContentProvider>> providers_;
};
