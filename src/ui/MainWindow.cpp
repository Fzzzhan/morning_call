#include "MainWindow.h"
#include "SettingsDialog.h"
#include "../utils/Logger.h"
#include "../services/PreferenceManager.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QDate>
#include <QApplication>
#include <QFile>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupUI();
    setupTrayIcon();

    // Load user profile
    PreferenceManager prefManager;
    userProfile_ = prefManager.loadProfile();

    loadTodaysFeed();

    LOG_INFO("MainWindow created successfully");
}

void MainWindow::setFeedGenerator(std::shared_ptr<FeedGenerator> generator) {
    feedGenerator_ = generator;
    LOG_INFO("FeedGenerator set for MainWindow");
}

void MainWindow::setUserProfile(const UserProfile& profile) {
    userProfile_ = profile;
    LOG_INFO("UserProfile updated in MainWindow");
}

void MainWindow::setupUI() {
    setWindowTitle("晨间认知补给器 - Morning Call");
    setMinimumSize(800, 600);
    resize(900, 700);

    // Load and set window icon
    QIcon appIcon;
    if (QFile::exists("resources/icons/morning_call.png")) {
        appIcon = QIcon("resources/icons/morning_call.png");
    } else if (QFile::exists("/usr/share/icons/hicolor/256x256/apps/morning_call.png")) {
        appIcon = QIcon("/usr/share/icons/hicolor/256x256/apps/morning_call.png");
    } else {
        appIcon = QIcon::fromTheme("applications-system");
    }
    setWindowIcon(appIcon);

    // Central widget
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    mainLayout_ = new QVBoxLayout(centralWidget_);
    mainLayout_->setContentsMargins(20, 20, 20, 20);
    mainLayout_->setSpacing(15);

    // Header section
    auto* headerLayout = new QHBoxLayout();

    titleLabel_ = new QLabel("晨间认知补给器", this);
    titleLabel_->setStyleSheet(
        "QLabel { "
        "  font-size: 24px; "
        "  font-weight: bold; "
        "  color: #333; "
        "}"
    );

    dateLabel_ = new QLabel(QDate::currentDate().toString("yyyy-MM-dd"), this);
    dateLabel_->setStyleSheet(
        "QLabel { "
        "  font-size: 16px; "
        "  color: #666; "
        "}"
    );

    refreshButton_ = new QPushButton("🔄 刷新", this);
    refreshButton_->setStyleSheet(
        "QPushButton { "
        "  background-color: #2196f3; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 4px; "
        "  padding: 8px 16px; "
        "  font-weight: bold; "
        "  font-size: 14px; "
        "} "
        "QPushButton:hover { "
        "  background-color: #1976d2; "
        "}"
    );
    connect(refreshButton_, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);

    settingsButton_ = new QPushButton("⚙️ 设置", this);
    settingsButton_->setStyleSheet(
        "QPushButton { "
        "  background-color: #757575; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 4px; "
        "  padding: 8px 16px; "
        "  font-weight: bold; "
        "  font-size: 14px; "
        "} "
        "QPushButton:hover { "
        "  background-color: #616161; "
        "}"
    );
    connect(settingsButton_, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);

    headerLayout->addWidget(titleLabel_);
    headerLayout->addWidget(dateLabel_);
    headerLayout->addStretch();
    headerLayout->addWidget(refreshButton_);
    headerLayout->addWidget(settingsButton_);

    mainLayout_->addLayout(headerLayout);

    // Scroll area for content cards
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setStyleSheet(
        "QScrollArea { "
        "  border: none; "
        "  background-color: #f5f5f5; "
        "}"
    );

    contentWidget_ = new QWidget(scrollArea_);
    contentLayout_ = new QVBoxLayout(contentWidget_);
    contentLayout_->setSpacing(15);
    contentLayout_->setContentsMargins(10, 10, 10, 10);

    scrollArea_->setWidget(contentWidget_);
    mainLayout_->addWidget(scrollArea_);

    // Set overall background
    setStyleSheet("QMainWindow { background-color: #fafafa; }");
}

void MainWindow::setupTrayIcon() {
    trayIcon_ = new QSystemTrayIcon(this);

    // Set tray icon (use 32x32 for better clarity in system tray)
    QIcon trayIconImg;
    if (QFile::exists("resources/icons/morning_call_32.png")) {
        trayIconImg = QIcon("resources/icons/morning_call_32.png");
    } else if (QFile::exists("resources/icons/morning_call.png")) {
        trayIconImg = QIcon("resources/icons/morning_call.png");
    } else if (QFile::exists("/usr/share/icons/hicolor/256x256/apps/morning_call.png")) {
        trayIconImg = QIcon("/usr/share/icons/hicolor/256x256/apps/morning_call.png");
    } else {
        trayIconImg = QIcon::fromTheme("applications-system");
    }
    trayIcon_->setIcon(trayIconImg);
    trayIcon_->setToolTip("晨间认知补给器 - Morning Call");

    // Create tray menu
    trayMenu_ = new QMenu(this);

    QAction* showAction = trayMenu_->addAction("显示主窗口");
    connect(showAction, &QAction::triggered, this, &MainWindow::onTrayShow);

    QAction* refreshAction = trayMenu_->addAction("刷新内容");
    connect(refreshAction, &QAction::triggered, this, &MainWindow::onTrayRefresh);

    trayMenu_->addSeparator();

    QAction* quitAction = trayMenu_->addAction("退出");
    connect(quitAction, &QAction::triggered, this, &MainWindow::onTrayQuit);

    trayIcon_->setContextMenu(trayMenu_);

    connect(trayIcon_, &QSystemTrayIcon::activated,
            this, &MainWindow::onTrayIconActivated);

    trayIcon_->show();

    LOG_INFO("System tray icon initialized");
}

void MainWindow::loadTodaysFeed() {
    clearCards();

    auto todaysFeed = repository_.getTodaysFeed();

    if (todaysFeed && !todaysFeed->empty()) {
        loadFeed(*todaysFeed);
        LOG_INFO("Loaded today's feed with {} items", todaysFeed->size());
    } else {
        QLabel* emptyLabel = new QLabel("今日内容为空\n点击刷新按钮生成新内容", contentWidget_);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet(
            "QLabel { "
            "  font-size: 16px; "
            "  color: #999; "
            "  padding: 50px; "
            "}"
        );
        contentLayout_->addWidget(emptyLabel);
        LOG_WARN("Today's feed is empty");
    }
}

void MainWindow::loadFeed(const DailyFeed& feed) {
    const auto& items = feed.getItems();

    for (const auto& item : items) {
        auto* card = new ContentCard(item, contentWidget_);

        connect(card, &ContentCard::favoriteClicked,
                this, &MainWindow::onFavorite);
        connect(card, &ContentCard::skipClicked,
                this, &MainWindow::onSkip);
        connect(card, &ContentCard::dislikeClicked,
                this, &MainWindow::onDislike);

        contentLayout_->addWidget(card);
        cards_.push_back(card);
    }

    contentLayout_->addStretch();
}

void MainWindow::clearCards() {
    // Remove all widgets from layout
    while (contentLayout_->count() > 0) {
        QLayoutItem* item = contentLayout_->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    cards_.clear();
}

void MainWindow::hideCard(const QString& contentId) {
    for (auto* card : cards_) {
        if (QString::fromStdString(card->getContentItem().getId()) == contentId) {
            card->hide();
            break;
        }
    }
}

void MainWindow::onRefreshClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认刷新",
        "确定要刷新今日内容吗？当前内容将被替换。",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        LOG_INFO("User requested feed refresh");

        if (!feedGenerator_) {
            LOG_ERROR("FeedGenerator not initialized");
            QMessageBox::warning(this, "错误", "Feed 生成器未初始化");
            return;
        }

        // Generate new feed
        try {
            auto newFeed = feedGenerator_->generateDailyFeed(userProfile_);

            if (newFeed.empty()) {
                LOG_WARN("Generated feed is empty");
                QMessageBox::warning(this, "提示", "生成的内容为空，请检查数据源");
                return;
            }

            LOG_INFO("Generated new feed with {} items", newFeed.size());

            // Reload the new feed
            loadTodaysFeed();

            trayIcon_->showMessage(
                "内容已刷新",
                QString("今日内容已更新（%1 条）").arg(newFeed.size()),
                QSystemTrayIcon::Information,
                3000
            );
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to generate feed: {}", e.what());
            QMessageBox::critical(this, "错误", QString("生成内容失败: %1").arg(e.what()));
        }
    }
}

void MainWindow::onSettingsClicked() {
    LOG_INFO("Settings button clicked");

    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        LOG_INFO("Settings saved, reloading feed");
        loadTodaysFeed();

        trayIcon_->showMessage(
            "设置已保存",
            "设置已更新",
            QSystemTrayIcon::Information,
            2000
        );
    }
}

void MainWindow::onFavorite(const QString& contentId) {
    std::string id = contentId.toStdString();

    if (repository_.recordUserAction(id, "favorite") &&
        repository_.addToFavorites(id)) {
        LOG_INFO("Content {} marked as favorite", id);

        trayIcon_->showMessage(
            "已收藏",
            "内容已添加到收藏夹",
            QSystemTrayIcon::Information,
            2000
        );
    } else {
        LOG_ERROR("Failed to favorite content {}", id);
    }
}

void MainWindow::onSkip(const QString& contentId) {
    std::string id = contentId.toStdString();

    if (repository_.recordUserAction(id, "skip")) {
        LOG_INFO("Content {} skipped", id);
        hideCard(contentId);
    } else {
        LOG_ERROR("Failed to skip content {}", id);
    }
}

void MainWindow::onDislike(const QString& contentId) {
    std::string id = contentId.toStdString();

    if (repository_.recordUserAction(id, "dislike")) {
        LOG_INFO("Content {} marked as disliked", id);
        hideCard(contentId);

        trayIcon_->showMessage(
            "已标记",
            "该类型内容将减少推荐",
            QSystemTrayIcon::Information,
            2000
        );
    } else {
        LOG_ERROR("Failed to dislike content {}", id);
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    // Minimize to tray instead of closing
    if (trayIcon_->isVisible()) {
        hide();
        trayIcon_->showMessage(
            "后台运行",
            "Morning Call 已最小化到系统托盘",
            QSystemTrayIcon::Information,
            2000
        );
        event->ignore();
        LOG_INFO("Window minimized to tray");
    } else {
        event->accept();
    }
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::DoubleClick) {
        show();
        raise();
        activateWindow();
        LOG_INFO("Window restored from tray (double-click)");
    }
}

void MainWindow::onTrayShow() {
    show();
    raise();
    activateWindow();
    LOG_INFO("Window restored from tray (menu)");
}

void MainWindow::onTrayRefresh() {
    LOG_INFO("Feed refresh requested from tray menu");

    if (!feedGenerator_) {
        LOG_ERROR("FeedGenerator not initialized");
        return;
    }

    try {
        auto newFeed = feedGenerator_->generateDailyFeed(userProfile_);

        if (!newFeed.empty()) {
            loadTodaysFeed();

            trayIcon_->showMessage(
                "内容已刷新",
                QString("今日内容已更新（%1 条）").arg(newFeed.size()),
                QSystemTrayIcon::Information,
                3000
            );
            LOG_INFO("Generated new feed with {} items from tray", newFeed.size());
        } else {
            LOG_WARN("Generated feed is empty");
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to generate feed from tray: {}", e.what());
    }
}

void MainWindow::onTrayQuit() {
    LOG_INFO("Application quit from tray menu");
    trayIcon_->hide();
    QApplication::quit();
}
