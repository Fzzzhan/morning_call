#pragma once

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QMenu>
#include <memory>
#include "../models/DailyFeed.h"
#include "../models/UserProfile.h"
#include "../data/ContentRepository.h"
#include "../core/FeedGenerator.h"
#include "ContentCard.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

    void loadTodaysFeed();
    void setFeedGenerator(std::shared_ptr<FeedGenerator> generator);
    void setUserProfile(const UserProfile& profile);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onRefreshClicked();
    void onSettingsClicked();
    void onFavorite(const QString& contentId);
    void onSkip(const QString& contentId);
    void onDislike(const QString& contentId);
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onTrayShow();
    void onTrayRefresh();
    void onTrayQuit();

private:
    void setupUI();
    void setupTrayIcon();
    void loadFeed(const DailyFeed& feed);
    void clearCards();
    void hideCard(const QString& contentId);

    // UI components
    QWidget* centralWidget_;
    QVBoxLayout* mainLayout_;
    QScrollArea* scrollArea_;
    QWidget* contentWidget_;
    QVBoxLayout* contentLayout_;
    QLabel* titleLabel_;
    QLabel* dateLabel_;
    QPushButton* refreshButton_;
    QPushButton* settingsButton_;

    // System tray
    QSystemTrayIcon* trayIcon_;
    QMenu* trayMenu_;

    // Data
    ContentRepository repository_;
    std::vector<ContentCard*> cards_;
    std::shared_ptr<FeedGenerator> feedGenerator_;
    UserProfile userProfile_;
};
