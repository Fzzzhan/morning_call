#include "ContentProvider.h"
#include "SeedContentLoader.h"
#include "RSSParser.h"
#include "../utils/Logger.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>
#include <QUuid>
#include <QTimer>

// ===== SeedContentProvider Implementation =====

SeedContentProvider::SeedContentProvider(const std::string& seedFilePath)
    : seedFilePath_(seedFilePath) {
    LOG_INFO("SeedContentProvider initialized with path: {}", seedFilePath_);
}

std::vector<ContentItem> SeedContentProvider::fetchContent() {
    SeedContentLoader loader;
    if (loader.loadFromFile(seedFilePath_)) {
        loaded_ = true;
        LOG_INFO("SeedContentProvider: Loaded {} items", loader.getAllContent().size());
        return loader.getAllContent();
    }

    LOG_ERROR("SeedContentProvider: Failed to load content from {}", seedFilePath_);
    loaded_ = false;
    return {};
}

bool SeedContentProvider::isAvailable() {
    // Check if seed file exists
    SeedContentLoader loader;
    return loader.loadFromFile(seedFilePath_);
}

// ===== LLMContentProvider Implementation (Placeholder) =====

LLMContentProvider::LLMContentProvider(const std::string& apiKey, const std::string& model)
    : apiKey_(apiKey), model_(model) {
    configured_ = !apiKey_.empty() && !model_.empty();
    if (configured_) {
        LOG_INFO("LLMContentProvider configured with model: {}", model_);
    } else {
        LOG_WARN("LLMContentProvider: Not configured (missing API key or model)");
    }
}

std::vector<ContentItem> LLMContentProvider::fetchContent() {
    LOG_WARN("LLMContentProvider::fetchContent() - Not implemented yet");

    // TODO: Future implementation
    // 1. Call LLM API (Claude, GPT, etc.)
    // 2. Use prompt to generate daily content in structured format
    // 3. Parse response into ContentItem objects
    // 4. Return generated content

    // Example prompt structure:
    // "Generate 10 pieces of morning inspiration content in Chinese.
    //  Categories: Thinking (cognitive insights), Habit (good practices),
    //  Knowledge (interesting facts), Trend (current trends).
    //  For each item provide: title, summary, why it matters, action suggestion.
    //  Return in JSON format."

    return {};  // Empty for now
}

bool LLMContentProvider::isAvailable() {
    return configured_;
}

// ===== RSSContentProvider Implementation (Placeholder) =====

RSSContentProvider::RSSContentProvider(const std::vector<std::string>& feedUrls)
    : feedUrls_(feedUrls) {
    if (!feedUrls_.empty()) {
        LOG_INFO("RSSContentProvider initialized with {} feed URLs", feedUrls_.size());
    } else {
        LOG_WARN("RSSContentProvider: No feed URLs provided");
    }
}

std::vector<ContentItem> RSSContentProvider::fetchContent() {
    LOG_INFO("RSSContentProvider: Fetching content from {} feed(s)", feedUrls_.size());

    std::vector<ContentItem> allContent;
    RSSParser parser;

    for (const auto& url : feedUrls_) {
        try {
            // Fetch RSS feed from network
            QString xmlContent = fetchFromNetwork(url);
            if (xmlContent.isEmpty()) {
                LOG_WARN("Failed to fetch RSS feed from: {}", url);
                continue;
            }

            // Parse RSS/Atom
            auto rssItems = parser.parse(xmlContent);
            LOG_INFO("Parsed {} items from {}", rssItems.size(), url);

            // Convert to ContentItem
            for (const auto& rssItem : rssItems) {
                auto contentItem = convertToContentItem(rssItem, url);
                if (!contentItem.getId().empty()) {
                    allContent.push_back(contentItem);
                }
            }

        } catch (const std::exception& e) {
            LOG_ERROR("Error processing RSS feed {}: {}", url, e.what());
        }
    }

    LOG_INFO("RSSContentProvider: Total {} items fetched", allContent.size());
    return allContent;
}

QString RSSContentProvider::fetchFromNetwork(const std::string& url) {
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl(QString::fromStdString(url)));

    // Set user agent
    request.setHeader(QNetworkRequest::UserAgentHeader, "MorningCall/1.0");
    request.setRawHeader("Accept", "application/rss+xml, application/atom+xml, application/xml, text/xml");

    QEventLoop loop;
    QNetworkReply* reply = manager.get(request);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    // Timeout after 10 seconds
    QTimer::singleShot(10000, &loop, &QEventLoop::quit);

    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QString content = QString::fromUtf8(reply->readAll());
        reply->deleteLater();
        return content;
    } else {
        LOG_ERROR("Network error fetching {}: {}", url, reply->errorString().toStdString());
        reply->deleteLater();
        return QString();
    }
}

ContentItem RSSContentProvider::convertToContentItem(const RSSItem& rssItem, const std::string& source) {
    // Generate unique ID
    QString id = QString("rss-%1").arg(QUuid::createUuid().toString(QUuid::WithoutBraces).left(8));

    // Classify based on keywords
    ContentCategory category = classifyContent(rssItem);

    // Use description as summary, limit length
    QString summary = rssItem.description;
    if (summary.length() > 200) {
        summary = summary.left(197) + "...";
    }

    // Extract or generate why_important
    QString whyImportant = generateWhyImportant(rssItem, category);

    // Generate action suggestion
    QString actionSuggestion = generateActionSuggestion(rssItem, category);

    // Calculate base score based on recency
    float baseScore = calculateFreshnessScore(rssItem.pubDate);

    ContentItem item(
        id.toStdString(),
        rssItem.title.toStdString(),
        category,
        summary.toStdString(),
        whyImportant.toStdString(),
        actionSuggestion.toStdString(),
        source,
        baseScore
    );

    return item;
}

ContentCategory RSSContentProvider::classifyContent(const RSSItem& rssItem) {
    QString text = (rssItem.title + " " + rssItem.description + " " + rssItem.category).toLower();

    // Thinking keywords
    QStringList thinkingKeywords = {
        "思维", "认知", "心理", "哲学", "逻辑", "思考", "决策",
        "thinking", "cognitive", "philosophy", "psychology", "logic"
    };

    // Habit keywords
    QStringList habitKeywords = {
        "习惯", "健康", "运动", "睡眠", "饮食", "效率", "时间管理",
        "habit", "health", "exercise", "sleep", "productivity", "routine"
    };

    // Knowledge keywords
    QStringList knowledgeKeywords = {
        "科学", "技术", "历史", "文化", "教育", "学习", "知识",
        "science", "technology", "history", "education", "learning"
    };

    // Trend keywords
    QStringList trendKeywords = {
        "趋势", "未来", "新", "创新", "AI", "人工智能", "区块链",
        "trend", "future", "innovation", "blockchain", "AI", "startup"
    };

    int thinkingScore = 0, habitScore = 0, knowledgeScore = 0, trendScore = 0;

    for (const auto& keyword : thinkingKeywords) {
        if (text.contains(keyword)) thinkingScore++;
    }
    for (const auto& keyword : habitKeywords) {
        if (text.contains(keyword)) habitScore++;
    }
    for (const auto& keyword : knowledgeKeywords) {
        if (text.contains(keyword)) knowledgeScore++;
    }
    for (const auto& keyword : trendKeywords) {
        if (text.contains(keyword)) trendScore++;
    }

    // Return category with highest score
    int maxScore = std::max({thinkingScore, habitScore, knowledgeScore, trendScore});

    if (maxScore == 0) {
        // Default to Knowledge if no keywords match
        return ContentCategory::Knowledge;
    }

    if (thinkingScore == maxScore) return ContentCategory::Thinking;
    if (habitScore == maxScore) return ContentCategory::Habit;
    if (trendScore == maxScore) return ContentCategory::Trend;
    return ContentCategory::Knowledge;
}

QString RSSContentProvider::generateWhyImportant(const RSSItem& rssItem, ContentCategory category) {
    // Simple generation based on category
    QString base = rssItem.description.left(100);

    switch (category) {
        case ContentCategory::Thinking:
            return QString("拓展认知边界，提升思维能力：%1").arg(base);
        case ContentCategory::Habit:
            return QString("养成良好习惯，提高生活质量：%1").arg(base);
        case ContentCategory::Knowledge:
            return QString("增长见识，丰富知识储备：%1").arg(base);
        case ContentCategory::Trend:
            return QString("把握趋势，保持行业敏锐度：%1").arg(base);
        default:
            return base;
    }
}

QString RSSContentProvider::generateActionSuggestion(const RSSItem& rssItem, ContentCategory category) {
    Q_UNUSED(rssItem);

    switch (category) {
        case ContentCategory::Thinking:
            return "今天花10分钟思考这个观点如何应用到你的工作生活中";
        case ContentCategory::Habit:
            return "今天尝试践行这个建议，记录你的感受";
        case ContentCategory::Knowledge:
            return "深入了解相关内容，分享给一个朋友";
        case ContentCategory::Trend:
            return "关注这个趋势，思考对你所在行业的影响";
        default:
            return "了解更多相关信息";
    }
}

float RSSContentProvider::calculateFreshnessScore(const QDateTime& pubDate) {
    if (!pubDate.isValid()) {
        return 0.5f;
    }

    qint64 hoursOld = pubDate.secsTo(QDateTime::currentDateTime()) / 3600;

    if (hoursOld < 24) return 0.9f;           // Last 24 hours
    if (hoursOld < 24 * 7) return 0.8f;       // Last week
    if (hoursOld < 24 * 30) return 0.7f;      // Last month
    return 0.6f;                               // Older
}

bool RSSContentProvider::isAvailable() {
    return !feedUrls_.empty();
}

// ===== ContentProviderManager Implementation =====

ContentProviderManager::ContentProviderManager() {
    LOG_INFO("ContentProviderManager initialized");
}

void ContentProviderManager::addProvider(std::shared_ptr<IContentProvider> provider) {
    if (provider && provider->isAvailable()) {
        providers_.push_back(provider);
        LOG_INFO("Added content provider: {}", provider->getProviderName());
    } else {
        LOG_WARN("Skipped unavailable provider: {}",
                 provider ? provider->getProviderName() : "nullptr");
    }
}

std::vector<ContentItem> ContentProviderManager::fetchAllContent() {
    std::vector<ContentItem> allContent;

    for (auto& provider : providers_) {
        try {
            auto content = provider->fetchContent();
            allContent.insert(allContent.end(), content.begin(), content.end());
            LOG_INFO("Fetched {} items from {}", content.size(), provider->getProviderName());
        } catch (const std::exception& e) {
            LOG_ERROR("Error fetching from {}: {}", provider->getProviderName(), e.what());
        }
    }

    LOG_INFO("Total content fetched from all providers: {}", allContent.size());
    return allContent;
}
