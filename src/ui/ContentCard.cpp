#include "ContentCard.h"
#include <QHBoxLayout>
#include <QFrame>

ContentCard::ContentCard(const ContentItem& item, QWidget* parent)
    : QWidget(parent), item_(item) {
    setupUI();
}

void ContentCard::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    // Card frame for visual separation
    setStyleSheet(
        "ContentCard { "
        "  background-color: white; "
        "  border: 1px solid #e0e0e0; "
        "  border-radius: 8px; "
        "}"
    );

    // Category badge
    categoryLabel_ = new QLabel(getCategoryDisplayName(), this);
    categoryLabel_->setStyleSheet(QString(
        "QLabel { "
        "  background-color: %1; "
        "  color: white; "
        "  padding: 4px 12px; "
        "  border-radius: 4px; "
        "  font-weight: bold; "
        "  font-size: 12px; "
        "}"
    ).arg(getCategoryColor()));
    categoryLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Title
    titleLabel_ = new QLabel(QString::fromStdString(item_.getTitle()), this);
    titleLabel_->setStyleSheet(
        "QLabel { "
        "  font-size: 18px; "
        "  font-weight: bold; "
        "  color: #333; "
        "}"
    );
    titleLabel_->setWordWrap(true);

    // Summary
    summaryLabel_ = new QLabel(QString::fromStdString(item_.getSummary()), this);
    summaryLabel_->setStyleSheet(
        "QLabel { "
        "  font-size: 14px; "
        "  color: #666; "
        "  line-height: 1.5; "
        "}"
    );
    summaryLabel_->setWordWrap(true);

    // Details section (collapsible)
    detailsLabel_ = new QLabel(this);
    detailsLabel_->setText(QString(
        "<p style='margin-bottom: 8px;'><b>为什么重要：</b>%1</p>"
    ).arg(QString::fromStdString(item_.getWhyImportant())));
    detailsLabel_->setStyleSheet(
        "QLabel { "
        "  font-size: 13px; "
        "  color: #555; "
        "  background-color: #f5f5f5; "
        "  padding: 10px; "
        "  border-radius: 4px; "
        "}"
    );
    detailsLabel_->setWordWrap(true);
    detailsLabel_->setVisible(false);

    // Action suggestion
    actionLabel_ = new QLabel(this);
    actionLabel_->setText(QString(
        "<p><b>今日行动：</b>%1</p>"
    ).arg(QString::fromStdString(item_.getActionSuggestion())));
    actionLabel_->setStyleSheet(
        "QLabel { "
        "  font-size: 13px; "
        "  color: #2c5aa0; "
        "  background-color: #e3f2fd; "
        "  padding: 10px; "
        "  border-radius: 4px; "
        "  border-left: 4px solid #2196f3; "
        "}"
    );
    actionLabel_->setWordWrap(true);
    actionLabel_->setVisible(false);

    // Toggle button
    toggleButton_ = new QPushButton("展开详情 ▼", this);
    toggleButton_->setStyleSheet(
        "QPushButton { "
        "  background-color: transparent; "
        "  border: none; "
        "  color: #2196f3; "
        "  font-size: 13px; "
        "  text-align: left; "
        "  padding: 5px; "
        "} "
        "QPushButton:hover { "
        "  color: #1976d2; "
        "  text-decoration: underline; "
        "}"
    );
    connect(toggleButton_, &QPushButton::clicked, this, &ContentCard::onToggleDetails);

    // Action buttons layout
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    favoriteButton_ = new QPushButton("⭐ 收藏", this);
    favoriteButton_->setStyleSheet(
        "QPushButton { "
        "  background-color: #ffc107; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 4px; "
        "  padding: 8px 16px; "
        "  font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "  background-color: #ffa000; "
        "}"
    );
    connect(favoriteButton_, &QPushButton::clicked, this, &ContentCard::onFavoriteClicked);

    skipButton_ = new QPushButton("⏭️ 跳过", this);
    skipButton_->setStyleSheet(
        "QPushButton { "
        "  background-color: #9e9e9e; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 4px; "
        "  padding: 8px 16px; "
        "  font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "  background-color: #757575; "
        "}"
    );
    connect(skipButton_, &QPushButton::clicked, this, &ContentCard::onSkipClicked);

    dislikeButton_ = new QPushButton("👎 不感兴趣", this);
    dislikeButton_->setStyleSheet(
        "QPushButton { "
        "  background-color: #f44336; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 4px; "
        "  padding: 8px 16px; "
        "  font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "  background-color: #d32f2f; "
        "}"
    );
    connect(dislikeButton_, &QPushButton::clicked, this, &ContentCard::onDislikeClicked);

    buttonLayout->addWidget(favoriteButton_);
    buttonLayout->addWidget(skipButton_);
    buttonLayout->addWidget(dislikeButton_);
    buttonLayout->addStretch();

    // Add all to main layout
    mainLayout->addWidget(categoryLabel_);
    mainLayout->addWidget(titleLabel_);
    mainLayout->addWidget(summaryLabel_);
    mainLayout->addWidget(toggleButton_);
    mainLayout->addWidget(detailsLabel_);
    mainLayout->addWidget(actionLabel_);
    mainLayout->addLayout(buttonLayout);
}

QString ContentCard::getCategoryColor() const {
    switch (item_.getCategory()) {
        case ContentCategory::Thinking:
            return "#9c27b0";  // Purple
        case ContentCategory::Habit:
            return "#4caf50";  // Green
        case ContentCategory::Knowledge:
            return "#2196f3";  // Blue
        case ContentCategory::Trend:
            return "#ff9800";  // Orange
        default:
            return "#757575";  // Grey
    }
}

QString ContentCard::getCategoryDisplayName() const {
    return QString::fromStdString(item_.getCategoryString());
}

void ContentCard::onToggleDetails() {
    detailsExpanded_ = !detailsExpanded_;
    detailsLabel_->setVisible(detailsExpanded_);
    actionLabel_->setVisible(detailsExpanded_);
    toggleButton_->setText(detailsExpanded_ ? "收起详情 ▲" : "展开详情 ▼");
}

void ContentCard::onFavoriteClicked() {
    emit favoriteClicked(QString::fromStdString(item_.getId()));
}

void ContentCard::onSkipClicked() {
    emit skipClicked(QString::fromStdString(item_.getId()));
}

void ContentCard::onDislikeClicked() {
    emit dislikeClicked(QString::fromStdString(item_.getId()));
}
