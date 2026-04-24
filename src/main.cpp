#include <QCoreApplication>
#include <QTimer>
#include <iostream>
#include <memory>

#include "utils/Logger.h"
#include "data/Database.h"
#include "data/ContentRepository.h"
#include "services/SeedContentLoader.h"
#include "services/ContentProvider.h"
#include "services/RSSFeedConfig.h"
#include "core/FeedGenerator.h"
#include "core/Scheduler.h"
#include "services/PreferenceManager.h"

void printFeed(const DailyFeed& feed) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "   晨间认知补给器 - Morning Call" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "日期: " << feed.getDate() << std::endl;
    std::cout << "内容数量: " << feed.size() << std::endl;
    std::cout << "========================================\n" << std::endl;

    const auto& items = feed.getItems();
    for (size_t i = 0; i < items.size(); ++i) {
        const auto& item = items[i];

        std::cout << "[" << (i + 1) << "] " << item.getCategoryString() << std::endl;
        std::cout << "标题: " << item.getTitle() << std::endl;
        std::cout << "摘要: " << item.getSummary() << std::endl;
        std::cout << "为什么重要: " << item.getWhyImportant() << std::endl;
        std::cout << "今日行动: " << item.getActionSuggestion() << std::endl;
        std::cout << "分数: " << item.getScore() << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }

    std::cout << "\n🎯 开始你的高效一天！\n" << std::endl;
}

int main(int argc, char *argv[]) {
    // Initialize Qt application
    QCoreApplication app(argc, argv);
    app.setApplicationName("Morning Call");
    app.setApplicationVersion("1.0.0");

    std::cout << "启动 Morning Call 应用..." << std::endl;

    // Initialize logger
    Logger::initialize();
    LOG_INFO("========================================");
    LOG_INFO("Morning Call Application Starting");
    LOG_INFO("========================================");

    // Initialize database
    auto& db = Database::getInstance();
    if (!db.initialize()) {
        LOG_ERROR("Failed to initialize database");
        std::cerr << "❌ 数据库初始化失败" << std::endl;
        return 1;
    }
    LOG_INFO("Database initialized successfully");
    std::cout << "✅ 数据库初始化成功" << std::endl;

    // Initialize feed generator
    auto feedGenerator = std::make_shared<FeedGenerator>();

    // Setup content providers
    ContentProviderManager providerManager;

    // 1. Add seed content provider
    std::string seedPath = SeedContentLoader::getDefaultSeedPath();
    LOG_INFO("Loading seed content from: {}", seedPath);
    std::cout << "📚 加载种子内容: " << seedPath << std::endl;

    auto seedProvider = std::make_shared<SeedContentProvider>(seedPath);
    providerManager.addProvider(seedProvider);

    // 2. Check for RSS feeds configuration
    std::string rssConfigPath = RSSFeedConfig::getDefaultConfigPath();
    RSSFeedConfig rssConfig;

    if (rssConfig.loadFromFile(rssConfigPath) && rssConfig.isEnabled()) {
        LOG_INFO("RSS feeds enabled, loading {} feed(s)", rssConfig.getFeedUrls().size());
        std::cout << "📡 RSS 订阅已启用（" << rssConfig.getFeedUrls().size() << " 个源）" << std::endl;

        auto rssProvider = std::make_shared<RSSContentProvider>(rssConfig.getFeedUrls());
        providerManager.addProvider(rssProvider);
    } else {
        LOG_INFO("RSS feeds disabled or not configured");
        std::cout << "ℹ️  RSS 订阅未启用（仅使用种子内容）" << std::endl;
    }

    // Initialize feed generator with all providers
    if (!feedGenerator->initializeWithProviders(providerManager)) {
        LOG_ERROR("Failed to initialize feed generator");
        std::cerr << "❌ Feed 生成器初始化失败" << std::endl;
        return 1;
    }
    LOG_INFO("Feed generator initialized successfully");
    std::cout << "✅ Feed 生成器初始化成功" << std::endl;

    // Load user preferences
    PreferenceManager prefManager;
    UserProfile profile = prefManager.loadProfile();
    LOG_INFO("User profile loaded");

    // Check if today's feed already exists
    ContentRepository contentRepo;
    auto todaysFeed = contentRepo.getTodaysFeed();

    if (!todaysFeed || todaysFeed->empty()) {
        // Generate today's feed
        LOG_INFO("Generating today's feed...");
        std::cout << "🔄 生成今日内容..." << std::endl;

        auto feed = feedGenerator->generateDailyFeed(profile);

        if (feed.empty()) {
            LOG_ERROR("Failed to generate feed - no items");
            std::cerr << "❌ 生成 Feed 失败：没有可用内容" << std::endl;
            return 1;
        }

        LOG_INFO("Today's feed generated with {} items", feed.size());
        std::cout << "✅ 今日内容生成成功（" << feed.size() << " 条）\n" << std::endl;

        // Print the feed
        printFeed(feed);
    } else {
        LOG_INFO("Today's feed already exists with {} items", todaysFeed->size());
        std::cout << "ℹ️  今日内容已存在（" << todaysFeed->size() << " 条）\n" << std::endl;

        // Print existing feed
        printFeed(*todaysFeed);
    }

    // Setup scheduler for automatic daily refresh
    auto scheduler = std::make_unique<Scheduler>();
    scheduler->setFeedGenerator(feedGenerator);
    scheduler->setUserProfile(profile);

    // Set callback for feed generation
    scheduler->setFeedGeneratedCallback([&contentRepo]() {
        LOG_INFO("Scheduler generated new feed, reloading...");
        auto newFeed = contentRepo.getTodaysFeed();
        if (newFeed) {
            printFeed(*newFeed);
        }
    });

    scheduler->start();
    LOG_INFO("Scheduler started, will check for daily refresh at {}", profile.getRefreshTime());
    std::cout << "⏰ 定时刷新已启动，刷新时间: " << profile.getRefreshTime() << std::endl;

    // Run event loop
    std::cout << "\n🚀 Morning Call 正在运行..." << std::endl;
    std::cout << "按 Ctrl+C 退出\n" << std::endl;

    return app.exec();
}
