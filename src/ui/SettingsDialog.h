#pragma once

#include <QDialog>
#include <QTimeEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "../services/PreferenceManager.h"

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog() override = default;

signals:
    void settingsSaved();

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onResetPreferencesClicked();
    void onCountChanged();

private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void updateTotalCount();

    // Refresh settings
    QTimeEdit* refreshTimeEdit_;

    // Content distribution
    QSpinBox* thinkingCountSpin_;
    QSpinBox* habitCountSpin_;
    QSpinBox* knowledgeCountSpin_;
    QSpinBox* trendCountSpin_;
    QLabel* totalCountLabel_;

    // Behavior settings
    QCheckBox* autoStartCheck_;
    QCheckBox* minimizeToTrayCheck_;

    // Appearance settings
    QComboBox* themeCombo_;

    // Buttons
    QPushButton* saveButton_;
    QPushButton* cancelButton_;
    QPushButton* resetPreferencesButton_;

    // Data
    PreferenceManager prefManager_;
};
