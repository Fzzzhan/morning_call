#pragma once

#include "../models/UserProfile.h"
#include "FeedGenerator.h"
#include <QObject>
#include <QTimer>
#include <memory>
#include <functional>

// Scheduler for automatic daily feed generation
class Scheduler : public QObject {
    Q_OBJECT

public:
    explicit Scheduler(QObject* parent = nullptr);
    ~Scheduler() override = default;

    // Set the feed generator
    void setFeedGenerator(std::shared_ptr<FeedGenerator> generator);

    // Set user profile for scheduling
    void setUserProfile(const UserProfile& profile);

    // Start/Stop scheduler
    void start();
    void stop();

    // Check if scheduler is running
    bool isRunning() const { return timer_->isActive(); }

    // Manually trigger feed generation
    void triggerNow();

    // Set callback for when feed is generated
    void setFeedGeneratedCallback(std::function<void()> callback);

signals:
    void feedGenerated();
    void schedulerStarted();
    void schedulerStopped();

private slots:
    void onTimerTick();

private:
    void checkAndGenerateFeed();
    bool shouldGenerateFeed();
    std::string getLastGenerationDate();
    void saveLastGenerationDate(const std::string& date);

    std::shared_ptr<FeedGenerator> feedGenerator_;
    std::unique_ptr<QTimer> timer_;
    UserProfile userProfile_;
    std::function<void()> feedGeneratedCallback_;

    static constexpr int CHECK_INTERVAL_MS = 60000;  // Check every minute
};
