#pragma once

#include <QString>
#include <QDateTime>
#include <vector>

struct RSSItem {
    QString title;
    QString link;
    QString description;
    QString content;      // Full content if available
    QDateTime pubDate;
    QString category;     // RSS category tag
    QString author;

    bool isValid() const {
        return !title.isEmpty() && !description.isEmpty();
    }
};

class RSSParser {
public:
    RSSParser() = default;

    // Parse RSS/Atom feed from XML string
    std::vector<RSSItem> parse(const QString& xmlContent);

    // Get last error message
    QString getLastError() const { return lastError_; }

private:
    // Parse RSS 2.0 format
    std::vector<RSSItem> parseRSS2(const QString& xmlContent);

    // Parse Atom format
    std::vector<RSSItem> parseAtom(const QString& xmlContent);

    // Helper: Extract text from XML element
    QString extractText(const QString& xml, const QString& tagName, int startPos = 0);

    // Helper: Extract CDATA content
    QString extractCDATA(const QString& content);

    // Helper: Parse RFC 822 or ISO 8601 date
    QDateTime parseDate(const QString& dateStr);

    QString lastError_;
};
