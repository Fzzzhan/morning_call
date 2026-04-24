#pragma once

#include <string>
#include <chrono>

enum class ContentCategory {
    Thinking,   // 思维/认知启发
    Habit,      // 好习惯
    Knowledge,  // 知识点
    Trend       // 热点/趋势
};

class ContentItem {
public:
    ContentItem() = default;
    ContentItem(const std::string& id, const std::string& title, ContentCategory category,
                const std::string& summary, const std::string& whyImportant,
                const std::string& actionSuggestion, const std::string& source, float baseScore);

    // Getters
    std::string getId() const { return id_; }
    std::string getTitle() const { return title_; }
    ContentCategory getCategory() const { return category_; }
    std::string getSummary() const { return summary_; }
    std::string getWhyImportant() const { return whyImportant_; }
    std::string getActionSuggestion() const { return actionSuggestion_; }
    std::string getSource() const { return source_; }
    float getScore() const { return score_; }
    float getBaseScore() const { return baseScore_; }
    std::chrono::system_clock::time_point getCreatedAt() const { return createdAt_; }
    int64_t getCreatedAtTimestamp() const;

    // Setters
    void setScore(float score) { score_ = score; }
    void setId(const std::string& id) { id_ = id; }
    void setTitle(const std::string& title) { title_ = title; }
    void setCategory(ContentCategory category) { category_ = category; }
    void setSummary(const std::string& summary) { summary_ = summary; }
    void setWhyImportant(const std::string& whyImportant) { whyImportant_ = whyImportant; }
    void setActionSuggestion(const std::string& actionSuggestion) { actionSuggestion_ = actionSuggestion; }
    void setSource(const std::string& source) { source_ = source; }
    void setBaseScore(float baseScore) { baseScore_ = baseScore; }
    void setCreatedAt(const std::chrono::system_clock::time_point& time) { createdAt_ = time; }
    void setCreatedAtFromTimestamp(int64_t timestamp);

    // Utility functions
    std::string getCategoryString() const;
    static ContentCategory categoryFromString(const std::string& str);
    static std::string categoryToString(ContentCategory category);

private:
    std::string id_;
    std::string title_;
    ContentCategory category_;
    std::string summary_;
    std::string whyImportant_;
    std::string actionSuggestion_;
    std::string source_;
    float score_{0.0f};
    float baseScore_{0.5f};
    std::chrono::system_clock::time_point createdAt_;
};
