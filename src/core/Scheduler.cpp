#include "Scheduler.h"
#include "../utils/Logger.h"
#include "../data/UserProfileRepository.h"
#include <QTime>
#include <QDate>

Scheduler::Scheduler(QObject* parent)
    : QObject(parent), timer_(std::make_unique<QTimer>(this)) {

    connect(timer_.get(), &QTimer::timeout, this, &Scheduler::onTimerTick);
    LOG_INFO("Scheduler created");
}

void Scheduler::setFeedGenerator(std::shared_ptr<FeedGenerator> generator) {
    feedGenerator_ = generator;
    LOG_INFO("Feed generator set for scheduler");
}

void Scheduler::setUserProfile(const UserProfile& profile) {
    userProfile_ = profile;
    LOG_INFO("User profile set for scheduler, refresh time: {}", profile.getRefreshTime());
}

void Scheduler::start() {
    if (!feedGenerator_) {
        LOG_ERROR("Cannot start scheduler: feed generator not set");
        return;
    }

    timer_->start(CHECK_INTERVAL_MS);
    LOG_INFO("Scheduler started, checking every {} ms", CHECK_INTERVAL_MS);
    emit schedulerStarted();
}

void Scheduler::stop() {
    timer_->stop();
    LOG_INFO("Scheduler stopped");
    emit schedulerStopped();
}

void Scheduler::triggerNow() {
    LOG_INFO("Manual feed generation triggered");
    checkAndGenerateFeed();
}

void Scheduler::setFeedGeneratedCallback(std::function<void()> callback) {
    feedGeneratedCallback_ = callback;
}

void Scheduler::onTimerTick() {
    if (shouldGenerateFeed()) {
        checkAndGenerateFeed();
    }
}

bool Scheduler::shouldGenerateFeed() {
    // Check if current time matches user's refresh time
    QTime currentTime = QTime::currentTime();
    QString refreshTimeStr = QString::fromStdString(userProfile_.getRefreshTime());
    QTime refreshTime = QTime::fromString(refreshTimeStr, "HH:mm");

    // Allow 1 minute window for the refresh time
    int minutesDiff = std::abs(currentTime.secsTo(refreshTime)) / 60;

    if (minutesDiff <= 1) {
        // Check if we already generated today
        std::string today = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
        std::string lastDate = getLastGenerationDate();

        if (lastDate != today) {
            LOG_INFO("Time to generate feed: current={}, target={}",
                     currentTime.toString("HH:mm").toStdString(),
                     refreshTimeStr.toStdString());
            return true;
        }
    }

    return false;
}

void Scheduler::checkAndGenerateFeed() {
    if (!feedGenerator_) {
        LOG_ERROR("Cannot generate feed: feed generator not set");
        return;
    }

    try {
        LOG_INFO("Generating daily feed...");
        feedGenerator_->generateDailyFeed(userProfile_);

        // Save generation date
        std::string today = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
        saveLastGenerationDate(today);

        LOG_INFO("Daily feed generated successfully");

        emit feedGenerated();

        if (feedGeneratedCallback_) {
            feedGeneratedCallback_();
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to generate daily feed: {}", e.what());
    }
}

std::string Scheduler::getLastGenerationDate() {
    UserProfileRepository repo;
    auto dateOpt = repo.getPreference("last_feed_generation");

    if (dateOpt) {
        return *dateOpt;
    }

    return "";  // No previous generation
}

void Scheduler::saveLastGenerationDate(const std::string& date) {
    UserProfileRepository repo;
    if (repo.savePreference("last_feed_generation", date)) {
        LOG_DEBUG("Saved last feed generation date: {}", date);
    } else {
        LOG_ERROR("Failed to save last feed generation date");
    }
}
