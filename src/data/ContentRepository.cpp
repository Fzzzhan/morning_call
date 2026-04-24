#include "ContentRepository.h"
#include "Database.h"
#include "../utils/Logger.h"
#include <QDate>
#include <sstream>

bool ContentRepository::saveContent(const ContentItem& item) {
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return false;
    }

    const char* sql = R"(
        INSERT OR REPLACE INTO contents
        (id, title, category, summary, why_important, action_suggestion, source, base_score, created_at)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return false;
    }

    sqlite3_bind_text(stmt, 1, item.getId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, item.getTitle().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, item.getCategoryString().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, item.getSummary().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, item.getWhyImportant().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, item.getActionSuggestion().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, item.getSource().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 8, item.getBaseScore());
    sqlite3_bind_int64(stmt, 9, item.getCreatedAtTimestamp());

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        LOG_ERROR("Failed to save content: {}", sqlite3_errmsg(conn));
        return false;
    }

    return true;
}

bool ContentRepository::saveContents(const std::vector<ContentItem>& items) {
    for (const auto& item : items) {
        if (!saveContent(item)) {
            return false;
        }
    }
    return true;
}

std::optional<ContentItem> ContentRepository::getContentById(const std::string& id) {
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return std::nullopt;
    }

    const char* sql = R"(
        SELECT id, title, category, summary, why_important, action_suggestion, source, base_score, created_at
        FROM contents WHERE id = ?
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return std::nullopt;
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        ContentItem item(
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            ContentItem::categoryFromString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)),
            sqlite3_column_double(stmt, 7)
        );
        item.setCreatedAtFromTimestamp(sqlite3_column_int64(stmt, 8));
        sqlite3_finalize(stmt);
        return item;
    }

    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::vector<ContentItem> ContentRepository::getAllContents() {
    std::vector<ContentItem> contents;
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return contents;
    }

    const char* sql = R"(
        SELECT id, title, category, summary, why_important, action_suggestion, source, base_score, created_at
        FROM contents ORDER BY created_at DESC
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return contents;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ContentItem item(
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            ContentItem::categoryFromString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)),
            sqlite3_column_double(stmt, 7)
        );
        item.setCreatedAtFromTimestamp(sqlite3_column_int64(stmt, 8));
        contents.push_back(item);
    }

    sqlite3_finalize(stmt);
    return contents;
}

std::vector<ContentItem> ContentRepository::getContentsByCategory(ContentCategory category) {
    std::vector<ContentItem> contents;
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return contents;
    }

    const char* sql = R"(
        SELECT id, title, category, summary, why_important, action_suggestion, source, base_score, created_at
        FROM contents WHERE category = ? ORDER BY created_at DESC
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return contents;
    }

    std::string categoryStr = ContentItem::categoryToString(category);
    sqlite3_bind_text(stmt, 1, categoryStr.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ContentItem item(
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            ContentItem::categoryFromString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)),
            sqlite3_column_double(stmt, 7)
        );
        item.setCreatedAtFromTimestamp(sqlite3_column_int64(stmt, 8));
        contents.push_back(item);
    }

    sqlite3_finalize(stmt);
    return contents;
}

bool ContentRepository::deleteContent(const std::string& id) {
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return false;
    }

    const char* sql = "DELETE FROM contents WHERE id = ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return false;
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        LOG_ERROR("Failed to delete content: {}", sqlite3_errmsg(conn));
        return false;
    }

    return true;
}

bool ContentRepository::saveDailyFeed(const DailyFeed& feed) {
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return false;
    }

    // Convert content IDs to comma-separated string
    std::string contentIdsStr;
    const auto& items = feed.getItems();
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) contentIdsStr += ",";
        contentIdsStr += items[i].getId();
    }

    const char* sql = R"(
        INSERT OR REPLACE INTO daily_feeds (date, content_ids, created_at)
        VALUES (?, ?, ?)
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return false;
    }

    sqlite3_bind_text(stmt, 1, feed.getDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, contentIdsStr.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, feed.getCreatedAtTimestamp());

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        LOG_ERROR("Failed to save daily feed: {}", sqlite3_errmsg(conn));
        return false;
    }

    return true;
}

std::optional<DailyFeed> ContentRepository::getDailyFeed(const std::string& date) {
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return std::nullopt;
    }

    const char* sql = "SELECT content_ids, created_at FROM daily_feeds WHERE date = ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return std::nullopt;
    }

    sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string contentIdsStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int64_t createdAt = sqlite3_column_int64(stmt, 1);
        sqlite3_finalize(stmt);

        // Parse comma-separated IDs
        std::vector<std::string> ids;
        std::stringstream ss(contentIdsStr);
        std::string id;
        while (std::getline(ss, id, ',')) {
            ids.push_back(id);
        }

        // Load content items
        std::vector<ContentItem> items;
        for (const auto& contentId : ids) {
            auto item = getContentById(contentId);
            if (item) {
                items.push_back(*item);
            }
        }

        DailyFeed feed(date, items);
        feed.setCreatedAtFromTimestamp(createdAt);
        return feed;
    }

    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::optional<DailyFeed> ContentRepository::getTodaysFeed() {
    return getDailyFeed(getTodayDate());
}

std::vector<std::string> ContentRepository::getShownContentIds() {
    std::vector<std::string> ids;
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return ids;
    }

    const char* sql = "SELECT content_ids FROM daily_feeds";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return ids;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string contentIdsStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        // Parse comma-separated IDs
        std::stringstream ss(contentIdsStr);
        std::string id;
        while (std::getline(ss, id, ',')) {
            ids.push_back(id);
        }
    }

    sqlite3_finalize(stmt);
    return ids;
}

bool ContentRepository::recordUserAction(const std::string& contentId, const std::string& actionType) {
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return false;
    }

    const char* sql = R"(
        INSERT INTO user_actions (content_id, action_type, created_at)
        VALUES (?, ?, ?)
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return false;
    }

    sqlite3_bind_text(stmt, 1, contentId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, actionType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, QDateTime::currentSecsSinceEpoch());

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        LOG_ERROR("Failed to record user action: {}", sqlite3_errmsg(conn));
        return false;
    }

    return true;
}

std::vector<std::string> ContentRepository::getActionsByType(const std::string& actionType) {
    std::vector<std::string> contentIds;
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return contentIds;
    }

    const char* sql = "SELECT DISTINCT content_id FROM user_actions WHERE action_type = ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return contentIds;
    }

    sqlite3_bind_text(stmt, 1, actionType.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        contentIds.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }

    sqlite3_finalize(stmt);
    return contentIds;
}

bool ContentRepository::isContentInFavorites(const std::string& contentId) {
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return false;
    }

    const char* sql = "SELECT COUNT(*) FROM favorites WHERE content_id = ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return false;
    }

    sqlite3_bind_text(stmt, 1, contentId.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count > 0;
}

bool ContentRepository::addToFavorites(const std::string& contentId) {
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return false;
    }

    const char* sql = R"(
        INSERT OR IGNORE INTO favorites (content_id, created_at)
        VALUES (?, ?)
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return false;
    }

    sqlite3_bind_text(stmt, 1, contentId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, QDateTime::currentSecsSinceEpoch());

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        LOG_ERROR("Failed to add to favorites: {}", sqlite3_errmsg(conn));
        return false;
    }

    return true;
}

bool ContentRepository::removeFromFavorites(const std::string& contentId) {
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return false;
    }

    const char* sql = "DELETE FROM favorites WHERE content_id = ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return false;
    }

    sqlite3_bind_text(stmt, 1, contentId.c_str(), -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        LOG_ERROR("Failed to remove from favorites: {}", sqlite3_errmsg(conn));
        return false;
    }

    return true;
}

std::vector<ContentItem> ContentRepository::getFavorites() {
    std::vector<ContentItem> favorites;
    auto& db = Database::getInstance();
    auto conn = db.getHandle();
    if (!conn) {
        LOG_ERROR("Failed to get database connection");
        return favorites;
    }

    const char* sql = R"(
        SELECT c.id, c.title, c.category, c.summary, c.why_important, c.action_suggestion, c.source, c.base_score, c.created_at
        FROM contents c
        INNER JOIN favorites f ON c.id = f.content_id
        ORDER BY f.created_at DESC
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(conn));
        return favorites;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ContentItem item(
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            ContentItem::categoryFromString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)),
            sqlite3_column_double(stmt, 7)
        );
        item.setCreatedAtFromTimestamp(sqlite3_column_int64(stmt, 8));
        favorites.push_back(item);
    }

    sqlite3_finalize(stmt);
    return favorites;
}

std::string ContentRepository::getTodayDate() {
    return QDate::currentDate().toString("yyyy-MM-dd").toStdString();
}
