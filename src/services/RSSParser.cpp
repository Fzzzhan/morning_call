#include "RSSParser.h"
#include "../utils/Logger.h"
#include <QRegularExpression>

std::vector<RSSItem> RSSParser::parse(const QString& xmlContent) {
    lastError_.clear();

    if (xmlContent.isEmpty()) {
        lastError_ = "Empty XML content";
        return {};
    }

    // Detect feed type
    if (xmlContent.contains("<rss", Qt::CaseInsensitive)) {
        LOG_DEBUG("Detected RSS 2.0 feed");
        return parseRSS2(xmlContent);
    } else if (xmlContent.contains("<feed", Qt::CaseInsensitive) &&
               xmlContent.contains("xmlns=\"http://www.w3.org/2005/Atom\"")) {
        LOG_DEBUG("Detected Atom feed");
        return parseAtom(xmlContent);
    } else {
        lastError_ = "Unknown feed format";
        LOG_WARN("Unknown feed format");
        return {};
    }
}

std::vector<RSSItem> RSSParser::parseRSS2(const QString& xmlContent) {
    std::vector<RSSItem> items;

    // Find all <item> elements
    QRegularExpression itemRegex("<item>(.*?)</item>",
                                  QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator it = itemRegex.globalMatch(xmlContent);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString itemXml = match.captured(1);

        RSSItem item;
        item.title = extractText(itemXml, "title");
        item.link = extractText(itemXml, "link");
        item.description = extractCDATA(extractText(itemXml, "description"));
        item.content = extractCDATA(extractText(itemXml, "content:encoded"));
        item.category = extractText(itemXml, "category");
        item.author = extractText(itemXml, "author");

        QString pubDateStr = extractText(itemXml, "pubDate");
        item.pubDate = parseDate(pubDateStr);

        if (item.isValid()) {
            items.push_back(item);
        }
    }

    LOG_INFO("Parsed {} items from RSS 2.0 feed", items.size());
    return items;
}

std::vector<RSSItem> RSSParser::parseAtom(const QString& xmlContent) {
    std::vector<RSSItem> items;

    // Find all <entry> elements
    QRegularExpression entryRegex("<entry>(.*?)</entry>",
                                   QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator it = entryRegex.globalMatch(xmlContent);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString entryXml = match.captured(1);

        RSSItem item;
        item.title = extractText(entryXml, "title");

        // Extract link href attribute
        QRegularExpression linkRegex("<link[^>]*href=[\"']([^\"']+)[\"']");
        QRegularExpressionMatch linkMatch = linkRegex.match(entryXml);
        if (linkMatch.hasMatch()) {
            item.link = linkMatch.captured(1);
        }

        item.description = extractText(entryXml, "summary");
        if (item.description.isEmpty()) {
            item.description = extractText(entryXml, "content");
        }
        item.content = extractText(entryXml, "content");

        // Atom uses <category term="..."/>
        QRegularExpression catRegex("<category[^>]*term=[\"']([^\"']+)[\"']");
        QRegularExpressionMatch catMatch = catRegex.match(entryXml);
        if (catMatch.hasMatch()) {
            item.category = catMatch.captured(1);
        }

        QString authorName = extractText(entryXml, "name");
        if (!authorName.isEmpty()) {
            item.author = authorName;
        }

        QString updated = extractText(entryXml, "updated");
        if (updated.isEmpty()) {
            updated = extractText(entryXml, "published");
        }
        item.pubDate = parseDate(updated);

        if (item.isValid()) {
            items.push_back(item);
        }
    }

    LOG_INFO("Parsed {} items from Atom feed", items.size());
    return items;
}

QString RSSParser::extractText(const QString& xml, const QString& tagName, int startPos) {
    QString openTag = QString("<%1>").arg(tagName);
    QString closeTag = QString("</%1>").arg(tagName);

    int start = xml.indexOf(openTag, startPos);
    if (start == -1) {
        return QString();
    }

    start += openTag.length();
    int end = xml.indexOf(closeTag, start);
    if (end == -1) {
        return QString();
    }

    QString content = xml.mid(start, end - start).trimmed();

    // Decode HTML entities
    content.replace("&lt;", "<");
    content.replace("&gt;", ">");
    content.replace("&amp;", "&");
    content.replace("&quot;", "\"");
    content.replace("&apos;", "'");

    return content;
}

QString RSSParser::extractCDATA(const QString& content) {
    if (content.contains("<![CDATA[")) {
        QRegularExpression cdataRegex("<!\\[CDATA\\[(.*?)\\]\\]>",
                                       QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatch match = cdataRegex.match(content);
        if (match.hasMatch()) {
            return match.captured(1).trimmed();
        }
    }
    return content;
}

QDateTime RSSParser::parseDate(const QString& dateStr) {
    if (dateStr.isEmpty()) {
        return QDateTime::currentDateTime();
    }

    // Try RFC 822 format (RSS 2.0): "Thu, 24 Apr 2026 10:00:00 GMT"
    QDateTime dt = QDateTime::fromString(dateStr, Qt::RFC2822Date);
    if (dt.isValid()) {
        return dt;
    }

    // Try ISO 8601 format (Atom): "2026-04-24T10:00:00Z"
    dt = QDateTime::fromString(dateStr, Qt::ISODate);
    if (dt.isValid()) {
        return dt;
    }

    // Try ISO 8601 with timezone
    dt = QDateTime::fromString(dateStr, "yyyy-MM-ddTHH:mm:ssZ");
    if (dt.isValid()) {
        return dt;
    }

    LOG_WARN("Failed to parse date: {}", dateStr.toStdString());
    return QDateTime::currentDateTime();
}
