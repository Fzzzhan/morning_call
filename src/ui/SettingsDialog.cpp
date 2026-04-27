#include "SettingsDialog.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QTime>

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent) {
    setupUI();
    loadSettings();
    LOG_INFO("SettingsDialog created");
}

void SettingsDialog::setupUI() {
    setWindowTitle("设置");
    setMinimumWidth(500);
    setModal(true);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // === Refresh Settings Group ===
    auto* refreshGroup = new QGroupBox("刷新设置", this);
    auto* refreshLayout = new QHBoxLayout(refreshGroup);

    auto* refreshLabel = new QLabel("每日刷新时间:", refreshGroup);
    refreshTimeEdit_ = new QTimeEdit(refreshGroup);
    refreshTimeEdit_->setDisplayFormat("HH:mm");
    refreshTimeEdit_->setTime(QTime(8, 0));

    refreshLayout->addWidget(refreshLabel);
    refreshLayout->addWidget(refreshTimeEdit_);
    refreshLayout->addStretch();

    mainLayout->addWidget(refreshGroup);

    // === Content Distribution Group ===
    auto* distributionGroup = new QGroupBox("内容配比（总计10条）", this);
    auto* distributionLayout = new QVBoxLayout(distributionGroup);

    auto* thinkingLayout = new QHBoxLayout();
    auto* thinkingLabel = new QLabel("思维启发:", distributionGroup);
    thinkingLabel->setMinimumWidth(80);
    thinkingCountSpin_ = new QSpinBox(distributionGroup);
    thinkingCountSpin_->setRange(0, 10);
    thinkingCountSpin_->setValue(3);
    thinkingLayout->addWidget(thinkingLabel);
    thinkingLayout->addWidget(thinkingCountSpin_);
    thinkingLayout->addStretch();

    auto* habitLayout = new QHBoxLayout();
    auto* habitLabel = new QLabel("好习惯:", distributionGroup);
    habitLabel->setMinimumWidth(80);
    habitCountSpin_ = new QSpinBox(distributionGroup);
    habitCountSpin_->setRange(0, 10);
    habitCountSpin_->setValue(2);
    habitLayout->addWidget(habitLabel);
    habitLayout->addWidget(habitCountSpin_);
    habitLayout->addStretch();

    auto* knowledgeLayout = new QHBoxLayout();
    auto* knowledgeLabel = new QLabel("知识点:", distributionGroup);
    knowledgeLabel->setMinimumWidth(80);
    knowledgeCountSpin_ = new QSpinBox(distributionGroup);
    knowledgeCountSpin_->setRange(0, 10);
    knowledgeCountSpin_->setValue(3);
    knowledgeLayout->addWidget(knowledgeLabel);
    knowledgeLayout->addWidget(knowledgeCountSpin_);
    knowledgeLayout->addStretch();

    auto* trendLayout = new QHBoxLayout();
    auto* trendLabel = new QLabel("热点趋势:", distributionGroup);
    trendLabel->setMinimumWidth(80);
    trendCountSpin_ = new QSpinBox(distributionGroup);
    trendCountSpin_->setRange(0, 10);
    trendCountSpin_->setValue(2);
    trendLayout->addWidget(trendLabel);
    trendLayout->addWidget(trendCountSpin_);
    trendLayout->addStretch();

    // Total count label
    auto* totalLayout = new QHBoxLayout();
    totalCountLabel_ = new QLabel("当前总计: 10 条", distributionGroup);
    totalCountLabel_->setStyleSheet("QLabel { font-weight: bold; color: #2196f3; }");
    totalLayout->addStretch();
    totalLayout->addWidget(totalCountLabel_);

    distributionLayout->addLayout(thinkingLayout);
    distributionLayout->addLayout(habitLayout);
    distributionLayout->addLayout(knowledgeLayout);
    distributionLayout->addLayout(trendLayout);
    distributionLayout->addLayout(totalLayout);

    // Connect spin boxes to update total
    connect(thinkingCountSpin_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onCountChanged);
    connect(habitCountSpin_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onCountChanged);
    connect(knowledgeCountSpin_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onCountChanged);
    connect(trendCountSpin_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsDialog::onCountChanged);

    mainLayout->addWidget(distributionGroup);

    // === Behavior Settings Group ===
    auto* behaviorGroup = new QGroupBox("行为设置", this);
    auto* behaviorLayout = new QVBoxLayout(behaviorGroup);

    autoStartCheck_ = new QCheckBox("开机自启动", behaviorGroup);
    minimizeToTrayCheck_ = new QCheckBox("关闭时最小化到系统托盘", behaviorGroup);
    minimizeToTrayCheck_->setChecked(true);

    behaviorLayout->addWidget(autoStartCheck_);
    behaviorLayout->addWidget(minimizeToTrayCheck_);

    mainLayout->addWidget(behaviorGroup);

    // === Appearance Settings Group ===
    auto* appearanceGroup = new QGroupBox("外观设置", this);
    auto* appearanceLayout = new QHBoxLayout(appearanceGroup);

    auto* themeLabel = new QLabel("主题模式:", appearanceGroup);
    themeCombo_ = new QComboBox(appearanceGroup);
    themeCombo_->addItem("浅色", "light");
    themeCombo_->addItem("深色", "dark");

    appearanceLayout->addWidget(themeLabel);
    appearanceLayout->addWidget(themeCombo_);
    appearanceLayout->addStretch();

    mainLayout->addWidget(appearanceGroup);

    // === Data Management ===
    auto* dataGroup = new QGroupBox("数据管理", this);
    auto* dataLayout = new QVBoxLayout(dataGroup);

    resetPreferencesButton_ = new QPushButton("重置推荐偏好", dataGroup);
    resetPreferencesButton_->setStyleSheet(
        "QPushButton { "
        "  background-color: #ff9800; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 4px; "
        "  padding: 8px 16px; "
        "  font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "  background-color: #f57c00; "
        "}"
    );
    connect(resetPreferencesButton_, &QPushButton::clicked,
            this, &SettingsDialog::onResetPreferencesClicked);

    auto* resetLabel = new QLabel("清空所有用户行为记录，重置类别推荐权重", dataGroup);
    resetLabel->setStyleSheet("QLabel { color: #666; font-size: 12px; }");

    dataLayout->addWidget(resetPreferencesButton_);
    dataLayout->addWidget(resetLabel);

    mainLayout->addWidget(dataGroup);

    // === Bottom Buttons ===
    mainLayout->addStretch();

    auto* buttonLayout = new QHBoxLayout();

    cancelButton_ = new QPushButton("取消", this);
    cancelButton_->setStyleSheet(
        "QPushButton { "
        "  background-color: #9e9e9e; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 4px; "
        "  padding: 10px 20px; "
        "  font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "  background-color: #757575; "
        "}"
    );
    connect(cancelButton_, &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);

    saveButton_ = new QPushButton("保存", this);
    saveButton_->setStyleSheet(
        "QPushButton { "
        "  background-color: #4caf50; "
        "  color: white; "
        "  border: none; "
        "  border-radius: 4px; "
        "  padding: 10px 20px; "
        "  font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "  background-color: #388e3c; "
        "}"
    );
    connect(saveButton_, &QPushButton::clicked, this, &SettingsDialog::onSaveClicked);

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton_);
    buttonLayout->addWidget(saveButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void SettingsDialog::loadSettings() {
    UserProfile profile = prefManager_.loadProfile();

    // Load refresh time
    QString refreshTime = QString::fromStdString(profile.getRefreshTime());
    refreshTimeEdit_->setTime(QTime::fromString(refreshTime, "HH:mm"));

    // Load content distribution
    thinkingCountSpin_->setValue(profile.getThinkingCount());
    habitCountSpin_->setValue(profile.getHabitCount());
    knowledgeCountSpin_->setValue(profile.getKnowledgeCount());
    trendCountSpin_->setValue(profile.getTrendCount());

    // Load behavior settings
    autoStartCheck_->setChecked(profile.getAutoStart());
    minimizeToTrayCheck_->setChecked(profile.getMinimizeToTray());

    // Load theme
    QString theme = QString::fromStdString(profile.getThemeMode());
    int themeIndex = themeCombo_->findData(theme);
    if (themeIndex >= 0) {
        themeCombo_->setCurrentIndex(themeIndex);
    }

    updateTotalCount();

    LOG_INFO("Settings loaded successfully");
}

void SettingsDialog::saveSettings() {
    UserProfile profile = prefManager_.loadProfile();

    // Save refresh time
    profile.setRefreshTime(refreshTimeEdit_->time().toString("HH:mm").toStdString());

    // Save content distribution
    profile.setThinkingCount(thinkingCountSpin_->value());
    profile.setHabitCount(habitCountSpin_->value());
    profile.setKnowledgeCount(knowledgeCountSpin_->value());
    profile.setTrendCount(trendCountSpin_->value());

    // Save behavior settings
    profile.setAutoStart(autoStartCheck_->isChecked());
    profile.setMinimizeToTray(minimizeToTrayCheck_->isChecked());

    // Save theme
    profile.setThemeMode(themeCombo_->currentData().toString().toStdString());

    if (prefManager_.saveProfile(profile)) {
        LOG_INFO("Settings saved successfully");
        emit settingsSaved();
    } else {
        LOG_ERROR("Failed to save settings");
        QMessageBox::critical(this, "错误", "保存设置失败");
    }
}

void SettingsDialog::onSaveClicked() {
    int totalCount = thinkingCountSpin_->value() +
                     habitCountSpin_->value() +
                     knowledgeCountSpin_->value() +
                     trendCountSpin_->value();

    if (totalCount != 10) {
        QMessageBox::warning(
            this,
            "配置错误",
            QString("内容总数必须为10条\n当前总计: %1 条").arg(totalCount)
        );
        return;
    }

    saveSettings();
    accept();
}

void SettingsDialog::onCancelClicked() {
    LOG_INFO("Settings dialog cancelled");
    reject();
}

void SettingsDialog::onResetPreferencesClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认重置",
        "确定要重置推荐偏好吗？\n\n这将清空所有用户行为记录，恢复默认推荐权重。\n此操作不可撤销。",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (prefManager_.resetPreferences()) {
            LOG_INFO("Preferences reset successfully");
            QMessageBox::information(this, "成功", "推荐偏好已重置");
        } else {
            LOG_ERROR("Failed to reset preferences");
            QMessageBox::critical(this, "错误", "重置偏好失败");
        }
    }
}

void SettingsDialog::onCountChanged() {
    updateTotalCount();
}

void SettingsDialog::updateTotalCount() {
    int total = thinkingCountSpin_->value() +
                habitCountSpin_->value() +
                knowledgeCountSpin_->value() +
                trendCountSpin_->value();

    QString color = (total == 10) ? "#4caf50" : "#f44336";
    totalCountLabel_->setText(QString("当前总计: %1 条").arg(total));
    totalCountLabel_->setStyleSheet(
        QString("QLabel { font-weight: bold; color: %1; }").arg(color)
    );
}
