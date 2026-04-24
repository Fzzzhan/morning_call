#include "ContentItem.h"
#include <stdexcept>

ContentItem::ContentItem(const std::string& id, const std::string& title, ContentCategory category,
                         const std::string& summary, const std::string& whyImportant,
                         const std::string& actionSuggestion, const std::string& source, float baseScore)
    : id_(id), title_(title), category_(category), summary_(summary),
      whyImportant_(whyImportant), actionSuggestion_(actionSuggestion),
      source_(source), baseScore_(baseScore), score_(baseScore) {
    createdAt_ = std::chrono::system_clock::now();
}

int64_t ContentItem::getCreatedAtTimestamp() const {
    return std::chrono::duration_cast<std::chrono::seconds>(
        createdAt_.time_since_epoch()).count();
}

void ContentItem::setCreatedAtFromTimestamp(int64_t timestamp) {
    createdAt_ = std::chrono::system_clock::time_point(
        std::chrono::seconds(timestamp));
}

std::string ContentItem::getCategoryString() const {
    return categoryToString(category_);
}

ContentCategory ContentItem::categoryFromString(const std::string& str) {
    if (str == "Thinking") return ContentCategory::Thinking;
    if (str == "Habit") return ContentCategory::Habit;
    if (str == "Knowledge") return ContentCategory::Knowledge;
    if (str == "Trend") return ContentCategory::Trend;
    throw std::invalid_argument("Invalid category string: " + str);
}

std::string ContentItem::categoryToString(ContentCategory category) {
    switch (category) {
        case ContentCategory::Thinking: return "Thinking";
        case ContentCategory::Habit: return "Habit";
        case ContentCategory::Knowledge: return "Knowledge";
        case ContentCategory::Trend: return "Trend";
    }
    return "Unknown";
}
