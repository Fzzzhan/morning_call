#include <QApplication>
#include "ui/MainWindow.h"
#include "utils/Logger.h"
#include "data/Database.h"
#include "services/SeedContentLoader.h"
#include "services/ContentProvider.h"
#include "services/RSSFeedConfig.h"
#include "core/FeedGenerator.h"
#include "services/PreferenceManager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Morning Call");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Morning Call");

    // Initialize logger
    Logger::initialize();
    LOG_INFO("========================================");
    LOG_INFO("Morning Call GUI Application Starting");
    LOG_INFO("========================================");

    // Initialize database
    auto& db = Database::getInstance();
    if (!db.initialize()) {
        LOG_ERROR("Failed to initialize database");
        return 1;
    }
    LOG_INFO("Database initialized successfully");

    // Initialize feed generator with content providers
    auto feedGenerator = std::make_shared<FeedGenerator>();

    // Setup content providers
    ContentProviderManager providerManager;

    // 1. Add seed content provider
    std::string seedPath = SeedContentLoader::getDefaultSeedPath();
    auto seedProvider = std::make_shared<SeedContentProvider>(seedPath);
    providerManager.addProvider(seedProvider);

    // 2. Check for RSS feeds configuration
    std::string rssConfigPath = RSSFeedConfig::getDefaultConfigPath();
    RSSFeedConfig rssConfig;

    if (rssConfig.loadFromFile(rssConfigPath) && rssConfig.isEnabled()) {
        LOG_INFO("RSS feeds enabled, loading {} feed(s)", rssConfig.getFeedUrls().size());
        auto rssProvider = std::make_shared<RSSContentProvider>(rssConfig.getFeedUrls());
        providerManager.addProvider(rssProvider);
    } else {
        LOG_INFO("RSS feeds disabled or not configured");
    }

    // Initialize feed generator with all providers
    if (!feedGenerator->initializeWithProviders(providerManager)) {
        LOG_ERROR("Failed to initialize feed generator");
        return 1;
    }
    LOG_INFO("Feed generator initialized successfully");

    // Check if we need to generate today's feed
    PreferenceManager prefManager;
    UserProfile profile = prefManager.loadProfile();

    // Create and show main window
    MainWindow window;

    // Set feed generator and user profile
    window.setFeedGenerator(feedGenerator);
    window.setUserProfile(profile);

    window.show();

    LOG_INFO("Main window displayed");

    return app.exec();
}
