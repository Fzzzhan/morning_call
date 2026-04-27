#include "Database.h"
#include <iostream>
#include <filesystem>
#include <sys/stat.h>

Database& Database::getInstance() {
    static Database instance;
    return instance;
}

std::string Database::getDatabasePath() {
    const char* home = getenv("HOME");
    if (!home) {
        return "";
    }
    
    std::string dataDir = std::string(home) + "/.local/share/morning_call";
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(dataDir);
    
    return dataDir + "/morning_call.db";
}

bool Database::initialize() {
    dbPath_ = getDatabasePath();
    if (dbPath_.empty()) {
        std::cerr << "Failed to determine database path" << std::endl;
        return false;
    }

    int rc = sqlite3_open(dbPath_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    // Enable foreign keys
    execute("PRAGMA foreign_keys = ON");

    return createTables();
}

bool Database::createTables() {
    const char* createContentsSql = R"(
        CREATE TABLE IF NOT EXISTS contents (
            id TEXT PRIMARY KEY,
            title TEXT NOT NULL,
            category TEXT NOT NULL,
            summary TEXT NOT NULL,
            why_important TEXT NOT NULL,
            action_suggestion TEXT NOT NULL,
            source TEXT NOT NULL,
            base_score REAL DEFAULT 0.5,
            created_at INTEGER NOT NULL
        )
    )";

    const char* createDailyFeedsSql = R"(
        CREATE TABLE IF NOT EXISTS daily_feeds (
            date TEXT PRIMARY KEY,
            content_ids TEXT NOT NULL,
            created_at INTEGER NOT NULL
        )
    )";

    const char* createUserActionsSql = R"(
        CREATE TABLE IF NOT EXISTS user_actions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            content_id TEXT NOT NULL,
            action_type TEXT NOT NULL,
            created_at INTEGER NOT NULL,
            FOREIGN KEY (content_id) REFERENCES contents(id)
        )
    )";

    const char* createUserPreferencesSql = R"(
        CREATE TABLE IF NOT EXISTS user_preferences (
            key TEXT PRIMARY KEY,
            value TEXT NOT NULL
        )
    )";

    const char* createFavoritesSql = R"(
        CREATE TABLE IF NOT EXISTS favorites (
            content_id TEXT PRIMARY KEY,
            created_at INTEGER NOT NULL,
            FOREIGN KEY (content_id) REFERENCES contents(id)
        )
    )";

    return execute(createContentsSql) &&
           execute(createDailyFeedsSql) &&
           execute(createUserActionsSql) &&
           execute(createUserPreferencesSql) &&
           execute(createFavoritesSql);
}

bool Database::execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

bool Database::beginTransaction() {
    return execute("BEGIN TRANSACTION");
}

bool Database::commit() {
    return execute("COMMIT");
}

bool Database::rollback() {
    return execute("ROLLBACK");
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}
