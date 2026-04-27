#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include "../models/ContentItem.h"

class ContentCard : public QWidget {
    Q_OBJECT

public:
    explicit ContentCard(const ContentItem& item, QWidget* parent = nullptr);
    ~ContentCard() override = default;

    const ContentItem& getContentItem() const { return item_; }

signals:
    void favoriteClicked(const QString& contentId);
    void skipClicked(const QString& contentId);
    void dislikeClicked(const QString& contentId);

private slots:
    void onFavoriteClicked();
    void onSkipClicked();
    void onDislikeClicked();
    void onToggleDetails();

private:
    void setupUI();
    QString getCategoryColor() const;
    QString getCategoryDisplayName() const;

    ContentItem item_;

    // UI components
    QLabel* categoryLabel_;
    QLabel* titleLabel_;
    QLabel* summaryLabel_;
    QLabel* detailsLabel_;
    QLabel* actionLabel_;
    QPushButton* toggleButton_;
    QPushButton* favoriteButton_;
    QPushButton* skipButton_;
    QPushButton* dislikeButton_;

    bool detailsExpanded_{false};
};
