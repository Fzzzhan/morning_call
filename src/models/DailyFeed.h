#pragma once

#include "ContentItem.h"
#include <vector>
#include <chrono>
#include <string>

class DailyFeed {
public:
    DailyFeed() = default;
    DailyFeed(const std::string& date, const std::vector<ContentItem>& items);

    // Getters
    std::string getDate() const { return date_; }
    std::vector<ContentItem> getItems() const { return items_; }
    std::chrono::system_clock::time_point getCreatedAt() const { return createdAt_; }
    int64_t getCreatedAtTimestamp() const;

    // Setters
    void setDate(const std::string& date) { date_ = date; }
    void setItems(const std::vector<ContentItem>& items) { items_ = items; }
    void addItem(const ContentItem& item) { items_.push_back(item); }
    void setCreatedAt(const std::chrono::system_clock::time_point& time) { createdAt_ = time; }
    void setCreatedAtFromTimestamp(int64_t timestamp);

    // Utility
    size_t size() const { return items_.size(); }
    bool empty() const { return items_.empty(); }

private:
    std::string date_;  // Format: YYYY-MM-DD
    std::vector<ContentItem> items_;
    std::chrono::system_clock::time_point createdAt_;
};
