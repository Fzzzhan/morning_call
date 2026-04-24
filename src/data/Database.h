#pragma once

#include <sqlite3.h>
#include <string>
#include <memory>

class Database {
public:
    static Database& getInstance();
    
    // Delete copy constructor and assignment operator
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool initialize();
    bool isOpen() const { return db_ != nullptr; }
    sqlite3* getHandle() { return db_; }

    // Transaction management
    bool beginTransaction();
    bool commit();
    bool rollback();

    // Execute simple queries
    bool execute(const std::string& sql);
    
    ~Database();

private:
    Database() = default;
    bool createTables();
    std::string getDatabasePath();

    sqlite3* db_{nullptr};
    std::string dbPath_;
};
