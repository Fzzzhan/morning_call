# Awake - Daily Cognitive Supplement

English | [中文](README_ZH.md)

A desktop application based on C++20 and Qt5 that provides 10 curated items daily including thinking insights, good habits, knowledge points, and trend observations.

**Current Version: 1.0.2**

## 🌟 Features

- ✅ **Smart Content Generation**: Automatically generates 10 daily items based on scoring algorithm (3 thinking + 2 habits + 3 knowledge + 2 trends)
- ✅ **User Preference Learning**: Records favorite/skip/dislike actions and dynamically adjusts recommendation weights
- ✅ **Auto Daily Refresh**: Configurable daily content refresh at fixed time
- ✅ **Content Persistence**: SQLite database stores all content and user behaviors
- ✅ **Graphical Interface**: Qt5 desktop GUI with card-style content display and system tray support
- ✅ **RSS Subscription**: Supports fetching RSS/Atom format content with automatic categorization
- ✅ **Settings Dialog**: Complete configuration interface for content ratio, refresh time, theme settings
- ✅ **Extensible Architecture**: Plugin-style content providers supporting seed content, RSS, LLM (reserved)

## 📦 Installation

### APT (Ubuntu/Debian) - Recommended for Linux

```bash
# Add PPA repository
sudo add-apt-repository ppa:fzzzzzz/awake
sudo apt update

# Install
sudo apt install awake

# Run
awake_gui
```

### Homebrew (macOS/Linux)

```bash
# Add tap
brew tap Fzzzhan/tap

# Install
brew install awake

# Run
awake_gui
```

### From Source

## 📋 Dependencies

### Ubuntu/Debian

```bash
# Qt 5 (Note: Project uses Qt5, not Qt6)
sudo apt install qtbase5-dev qttools5-dev qttools5-dev-tools

# SQLite3
sudo apt install libsqlite3-dev

# nlohmann-json
sudo apt install nlohmann-json3-dev

# spdlog
sudo apt install libspdlog-dev

# Build tools
sudo apt install cmake g++ ninja-build
```

**Note**: This project uses Qt5. If your system only has Qt5 without development packages, the project is configured to use bundled third-party libraries (located in `third_party/include/`).

## 🔨 Build

```bash
# Configure project
cmake -B build -G Ninja

# Build (generates two executables)
cmake --build build

# Run console version
./build/awake

# Run GUI version (recommended)
./build/awake_gui
```

After building, two programs are generated:
- **awake** - Console version, displays content in terminal
- **awake_gui** - GUI version with graphical interface

## 📦 System Installation

### Install to System

Install the program to system path for global access:

```bash
# Install to system (requires sudo)
sudo cmake --install build

# Or specify installation prefix (no sudo needed)
cmake --install build --prefix ~/.local
```

**Installation includes:**
- Executables → `/usr/local/bin/` or `~/.local/bin/`
  - `awake` - Console version
  - `awake_gui` - GUI version
- Desktop launcher → `/usr/local/share/applications/awake.desktop`
- Application icon → `/usr/local/share/icons/hicolor/256x256/apps/awake.png`

### Usage After Installation

```bash
# Run directly from any directory
awake_gui

# Or launch from application menu
# Open Applications → Utility → Awake
```

### Add to PATH (if using ~/.local installation)

If installed to `~/.local`, ensure the path is in your PATH:

```bash
# Add to ~/.bashrc or ~/.zshrc
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

### Uninstall

```bash
# Uninstall from system
sudo rm /usr/local/bin/awake
sudo rm /usr/local/bin/awake_gui
sudo rm /usr/local/share/applications/awake.desktop
sudo rm /usr/local/share/icons/hicolor/256x256/apps/awake.png

# Or uninstall from ~/.local
rm ~/.local/bin/awake*
rm ~/.local/share/applications/awake.desktop
rm ~/.local/share/icons/hicolor/256x256/apps/awake.png

# Update desktop database
update-desktop-database ~/.local/share/applications/ 2>/dev/null || true
```

### Auto-start on Boot (Optional)

```bash
# Method 1: Add via desktop environment's "Startup Applications" settings

# Method 2: Manually create autostart file
mkdir -p ~/.config/autostart
cp /usr/local/share/applications/awake.desktop ~/.config/autostart/
```

## 🚀 Quick Start

### First Run

```bash
cd /home/fengze/Github/awake
./build/awake
```

The application will:
1. Initialize database (located at `~/.local/share/awake/awake.db`)
2. Load seed content (100 high-quality Chinese items)
3. Generate today's 10 items
4. Print content in terminal
5. Start scheduled refresh (default 08:00 daily)

### Output Example

```
Starting Awake application...
✅ Database initialized successfully
📚 Loading seed content: resources/seed_content.json
✅ Feed generator initialized successfully
🔄 Generating today's content...
✅ Today's content generated successfully (10 items)

========================================
   Awake - Daily Cognitive Supplement
========================================
Date: 2026-04-24
Content count: 10
========================================

[1] Thinking
Title: Sunk Cost Fallacy
Summary: Already invested costs should not affect future decisions
Why Important: Avoid making irrational decisions due to past investments, learn to cut losses timely
Today's Action: Review a project you continue because you've 'invested too much' and evaluate if it's truly worth continuing
Score: 0.850000
----------------------------------------

...(more content)

🎯 Start your productive day!

⏰ Scheduled refresh activated, refresh time: 08:00
🚀 Awake is running...
Press Ctrl+C to exit
```

## 📁 Project Structure

```
awake/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── resources/
│   └── seed_content.json   # Seed content (100 items)
├── src/
│   ├── main.cpp            # Program entry
│   ├── models/             # Data models
│   │   ├── ContentItem.*
│   │   ├── UserProfile.*
│   │   └── DailyFeed.*
│   ├── data/               # Data access layer
│   │   ├── Database.*
│   │   ├── ContentRepository.*
│   │   └── UserProfileRepository.*
│   ├── core/               # Core business logic
│   │   ├── FeedGenerator.* # Feed generator
│   │   ├── ContentScorer.* # Content scorer
│   │   └── Scheduler.*     # Scheduler
│   ├── services/           # Service layer
│   │   ├── SeedContentLoader.*
│   │   ├── ContentProvider.*
│   │   └── PreferenceManager.*
│   └── utils/              # Utilities
│       ├── Logger.*
│       └── JsonHelper.*
└── docs/                   # Documentation (to be added)
```

## 🧠 Core Concepts

### Content Scoring Algorithm

```cpp
Final Score = baseScore × freshnessWeight × preferenceWeight

Where:
- baseScore: Inherent content quality (0.5-1.0, preset)
- freshnessWeight: Freshness weight (linear decay over time)
- preferenceWeight: User preference weight (based on historical behavior)
```

### Preference Learning

- Favorite: Category weight +0.3
- Skip: Category weight -0.1
- Dislike: Category weight -0.5

### Content Selection Strategy

1. Get all available content
2. Filter already displayed content
3. Calculate score for each item
4. Sort by score
5. Select top N items according to configured ratio (default 3:2:3:2)

## 📡 RSS Subscription (Integrated)

The project has fully implemented RSS/Atom subscription functionality to fetch content from the web and automatically mix with seed content.

### Quick Enable RSS

1. **Edit Configuration File**

```bash
# Edit RSS feed config (file already exists)
nano resources/rss_feeds.txt

# Uncomment desired feeds or add new URLs
# https://www.36kr.com/feed
# https://sspai.com/feed
```

2. **RSS Auto-loading**

The application automatically detects `resources/rss_feeds.txt` on startup:
- If file is empty or all comments: Use seed content only (default)
- If valid URLs exist: Automatically fetch RSS content and mix with seed content

**No code modification needed** - just edit the config file to enable/disable RSS.

### Configuration Example

```txt
# Tech news
https://www.36kr.com/feed
https://sspai.com/feed

# Knowledge learning
https://www.zhihu.com/rss
```

### RSS Features

- ✅ Supports RSS 2.0 and Atom formats
- ✅ Auto-parses title, description, link, publish time
- ✅ Keyword-based auto-categorization (thinking/habit/knowledge/trend)
- ✅ Smart generation of "why important" and "today's action"
- ✅ Freshness scoring based on publish time
- ✅ Handles CDATA and HTML entities
- ✅ Supports multiple RSS feed aggregation

### Content Classification Keywords

RSS content is automatically categorized based on keywords:

- **Thinking**: thinking, cognition, psychology, philosophy, logic, decision
- **Habit**: habit, health, exercise, sleep, efficiency, time management
- **Knowledge**: science, technology, history, culture, education, learning
- **Trend**: trend, future, innovation, AI, blockchain, startup

## 🎯 Data Storage

### Database Location
```
~/.local/share/awake/awake.db
```

### Log Location
```
~/.local/share/awake/logs/awake.log
```

### Database Schema

```sql
-- Contents table
CREATE TABLE contents (
    id TEXT PRIMARY KEY,
    title TEXT NOT NULL,
    category TEXT NOT NULL,
    summary TEXT NOT NULL,
    why_important TEXT NOT NULL,
    action_suggestion TEXT NOT NULL,
    source TEXT NOT NULL,
    base_score REAL DEFAULT 0.5,
    created_at INTEGER NOT NULL
);

-- Daily feeds table
CREATE TABLE daily_feeds (
    date TEXT PRIMARY KEY,
    content_ids TEXT NOT NULL,
    created_at INTEGER NOT NULL
);

-- User actions table
CREATE TABLE user_actions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    content_id TEXT NOT NULL,
    action_type TEXT NOT NULL,  -- 'favorite', 'skip', 'dislike'
    created_at INTEGER NOT NULL
);

-- User preferences table
CREATE TABLE user_preferences (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
);

-- Favorites table
CREATE TABLE favorites (
    content_id TEXT PRIMARY KEY,
    created_at INTEGER NOT NULL
);
```

## 🔧 Configuration Options

Current version configures via database `user_preferences` table:

- `thinking_count`: Thinking insight count (default 3)
- `habit_count`: Good habit count (default 2)
- `knowledge_count`: Knowledge point count (default 3)
- `trend_count`: Trend content count (default 2)
- `refresh_time`: Daily refresh time (default "08:00")
- `auto_start`: Auto-start on boot (default false)
- `minimize_to_tray`: Minimize to tray (default true)
- `theme_mode`: Theme mode (default "light")

## 📈 Future Extensions

Project architecture has reserved the following extension points:

### 1. LLM Content Generation

```cpp
class LLMContentProvider : public IContentProvider {
    // Use Claude/GPT API to generate daily content
    // Or summarize and categorize RSS content
};
```

### 2. RSS Subscription

```cpp
class RSSContentProvider : public IContentProvider {
    // Fetch RSS feeds
    // Use LLM for summarization and categorization
};
```

### 3. Mixed Content Strategy

```cpp
ContentProviderManager manager;
manager.addProvider(std::make_shared<SeedContentProvider>(seedPath));
manager.addProvider(std::make_shared<LLMContentProvider>(apiKey));
manager.addProvider(std::make_shared<RSSContentProvider>(feedUrls));
```

### 4. GUI Interface

Current version is command-line, future additions:
- Qt Widgets desktop GUI
- System tray icon
- Settings dialog
- Content card display

## 🐛 Debugging

### View Logs

```bash
tail -f ~/.local/share/awake/logs/awake.log
```

### Clear Database and Restart

```bash
rm ~/.local/share/awake/awake.db
./build/awake
```

### Test Content Generation

Modify code in `main.cpp` to manually trigger generation:

```cpp
auto feed = feedGenerator->generateDailyFeed(profile);
printFeed(feed);
```

## ❓ FAQ

### Q: Can't find Qt5 during build?
A: Ensure qtbase5-dev is installed, check with `apt list --installed | grep qt5`.

### Q: Can't find seed content file at runtime?
A: Ensure `resources/seed_content.json` exists at the relative path from the running directory.

### Q: How to modify refresh time?
A: Currently requires direct database modification, or wait for GUI settings interface.

### Q: Will content repeat?
A: No. The system records all displayed content IDs and won't recommend duplicates.

## 📝 Development Roadmap

- [ ] Create complete Qt GUI interface
- [ ] Implement settings dialog
- [ ] Add system tray functionality
- [ ] Implement LLM content generation
- [ ] Add RSS subscription feature
- [ ] Support import/export configuration
- [ ] Cross-device sync (WebDAV)

## 📄 License

MIT

## 🤝 Contributing

Issues and Pull Requests are welcome!

## Author

Fengze Han
