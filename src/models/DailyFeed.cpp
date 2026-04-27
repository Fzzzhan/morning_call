#include "DailyFeed.h"

DailyFeed::DailyFeed(const std::string& date, const std::vector<ContentItem>& items)
    : date_(date), items_(items) {
    createdAt_ = std::chrono::system_clock::now();
}

int64_t DailyFeed::getCreatedAtTimestamp() const {
    return std::chrono::duration_cast<std::chrono::seconds>(
        createdAt_.time_since_epoch()).count();
}

void DailyFeed::setCreatedAtFromTimestamp(int64_t timestamp) {
    createdAt_ = std::chrono::system_clock::time_point(
        std::chrono::seconds(timestamp));
}
