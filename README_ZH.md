# Awake - 晨间认知补给器

[English](README.md) | 中文

一个基于C++20和Qt6的桌面应用，每天为你提供10条精选的思维启发、好习惯、知识点和趋势洞察。

## 🌟 功能特点

- ✅ **智能内容生成**：基于评分算法自动生成每日10条内容（3思维+2习惯+3知识+2趋势）
- ✅ **用户偏好学习**：记录收藏/跳过/不感兴趣行为，动态调整推荐权重
- ✅ **自动每日刷新**：可配置每天固定时间自动生成新内容
- ✅ **内容持久化**：SQLite数据库存储所有内容和用户行为
- ✅ **图形化界面**：Qt5 桌面 GUI，卡片式内容展示，系统托盘支持
- ✅ **RSS 订阅源**：支持抓取 RSS/Atom 格式的网络内容，自动分类
- ✅ **设置对话框**：完整的配置界面，支持内容配比、刷新时间、主题等设置
- ✅ **可扩展架构**：插件式内容提供者，支持种子内容、RSS、LLM（预留）

## 📋 依赖项

### Ubuntu/Debian

```bash
# Qt 5 (注：项目使用 Qt5，不是 Qt6)
sudo apt install qtbase5-dev qttools5-dev qttools5-dev-tools

# SQLite3
sudo apt install libsqlite3-dev

# nlohmann-json
sudo apt install nlohmann-json3-dev

# spdlog
sudo apt install libspdlog-dev

# 构建工具
sudo apt install cmake g++ ninja-build
```

**注意**：本项目使用 Qt5。如果系统只有 Qt5 而没有开发包，项目已配置使用捆绑的第三方库（位于 `third_party/include/`）。

## 🔨 编译

```bash
# 配置项目
cmake -B build -G Ninja

# 编译（会生成两个可执行文件）
cmake --build build

# 运行控制台版本
./build/awake

# 运行 GUI 版本（推荐）
./build/awake_gui
```

编译完成后会生成两个程序：
- **awake** - 控制台版本，在终端显示内容
- **awake_gui** - GUI 版本，图形化界面

## 📦 系统安装

### 安装到系统

将程序安装到系统路径，方便全局访问：

```bash
# 安装到系统（需要 sudo 权限）
sudo cmake --install build

# 或者指定安装前缀（无需 sudo）
cmake --install build --prefix ~/.local
```

**安装内容：**
- 可执行文件 → `/usr/local/bin/` 或 `~/.local/bin/`
  - `awake` - 控制台版本
  - `awake_gui` - GUI 版本
- 桌面启动器 → `/usr/local/share/applications/awake.desktop`
- 应用图标 → `/usr/local/share/icons/hicolor/256x256/apps/awake.png`

### 安装后使用

```bash
# 在任何目录直接运行
awake_gui

# 或从应用菜单启动
# 打开应用程序 → Utility → Awake
```

### 添加到 PATH（如果使用 ~/.local 安装）

如果安装到 `~/.local`，需要确保该路径在 PATH 中：

```bash
# 添加到 ~/.bashrc 或 ~/.zshrc
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

### 卸载

```bash
# 从系统卸载
sudo rm /usr/local/bin/awake
sudo rm /usr/local/bin/awake_gui
sudo rm /usr/local/share/applications/awake.desktop
sudo rm /usr/local/share/icons/hicolor/256x256/apps/awake.png

# 或从 ~/.local 卸载
rm ~/.local/bin/awake*
rm ~/.local/share/applications/awake.desktop
rm ~/.local/share/icons/hicolor/256x256/apps/awake.png

# 更新桌面数据库
update-desktop-database ~/.local/share/applications/ 2>/dev/null || true
```

### 开机自启动（可选）

```bash
# 方法1：通过桌面环境的"启动应用程序"设置添加

# 方法2：手动创建 autostart 文件
mkdir -p ~/.config/autostart
cp /usr/local/share/applications/awake.desktop ~/.config/autostart/
```

## 🚀 快速开始

### 首次运行

```bash
cd /home/fengze/Github/awake
./build/awake
```

应用将：
1. 初始化数据库（位于 `~/.local/share/awake/awake.db`）
2. 加载种子内容（100条高质量中文内容）
3. 生成今日的10条内容
4. 在终端打印内容
5. 启动定时刷新（默认每天08:00）

### 输出示例

```
启动 Awake 应用...
✅ 数据库初始化成功
📚 加载种子内容: resources/seed_content.json
✅ Feed 生成器初始化成功
🔄 生成今日内容...
✅ 今日内容生成成功（10 条）

========================================
   晨间认知补给器 - Awake
========================================
日期: 2026-04-24
内容数量: 10
========================================

[1] Thinking
标题: 沉没成本谬误
摘要: 已经投入的成本不应该影响未来的决策
为什么重要: 避免因为过去的投入而做出不理性的决策，学会及时止损
今日行动: 今天审视一个你因为'已经投入太多'而继续的项目，评估是否真的值得继续
分数: 0.850000
----------------------------------------

...（更多内容）

🎯 开始你的高效一天！

⏰ 定时刷新已启动，刷新时间: 08:00
🚀 Awake 正在运行...
按 Ctrl+C 退出
```

## 📁 项目结构

```
awake/
├── CMakeLists.txt          # 构建配置
├── README.md               # 本文件
├── resources/
│   └── seed_content.json   # 种子内容（100条）
├── src/
│   ├── main.cpp            # 程序入口
│   ├── models/             # 数据模型
│   │   ├── ContentItem.*
│   │   ├── UserProfile.*
│   │   └── DailyFeed.*
│   ├── data/               # 数据访问层
│   │   ├── Database.*
│   │   ├── ContentRepository.*
│   │   └── UserProfileRepository.*
│   ├── core/               # 核心业务逻辑
│   │   ├── FeedGenerator.* # Feed生成器
│   │   ├── ContentScorer.* # 内容评分器
│   │   └── Scheduler.*     # 定时调度器
│   ├── services/           # 服务层
│   │   ├── SeedContentLoader.*
│   │   ├── ContentProvider.*
│   │   └── PreferenceManager.*
│   └── utils/              # 工具类
│       ├── Logger.*
│       └── JsonHelper.*
└── docs/                   # 文档（待添加）
```

## 🧠 核心概念

### 内容评分算法

```cpp
最终分数 = baseScore × freshnessWeight × preferenceWeight

其中：
- baseScore: 内容固有质量（0.5-1.0，预设）
- freshnessWeight: 新鲜度权重（随时间线性衰减）
- preferenceWeight: 用户偏好权重（基于历史行为）
```

### 偏好学习

- 收藏：该类别权重 +0.3
- 跳过：该类别权重 -0.1
- 不感兴趣：该类别权重 -0.5

### 内容选择策略

1. 获取所有可用内容
2. 过滤已展示过的内容
3. 计算每条内容的评分
4. 按评分排序
5. 根据配置的比例（默认3:2:3:2）选择前N条

## 📡 RSS 订阅源（已集成）

项目已完整实现 RSS/Atom 订阅功能，可以从网络抓取内容并自动与种子内容混合展示。

### 快速启用 RSS

1. **编辑配置文件**

```bash
# 编辑 RSS 源配置（文件已存在）
nano resources/rss_feeds.txt

# 取消注释你想要的源，或添加新的 URL
# https://www.36kr.com/feed
# https://sspai.com/feed
```

2. **RSS 自动加载**

应用启动时会自动检测 `resources/rss_feeds.txt`：
- 如果文件为空或全是注释：只使用种子内容（默认）
- 如果有有效 URL：自动抓取 RSS 内容并与种子内容混合

**无需修改代码**，只需编辑配置文件即可启用/禁用 RSS。

### 配置示例

```txt
# 科技新闻
https://www.36kr.com/feed
https://sspai.com/feed

# 知识学习
https://www.zhihu.com/rss
```

### RSS 功能特性

- ✅ 支持 RSS 2.0 和 Atom 格式
- ✅ 自动解析标题、描述、链接、发布时间
- ✅ 基于关键词自动分类（思维/习惯/知识/趋势）
- ✅ 智能生成"为什么重要"和"今日行动"
- ✅ 基于发布时间计算新鲜度评分
- ✅ 处理 CDATA 和 HTML 实体
- ✅ 支持多个 RSS 源聚合

### 内容分类关键词

RSS 内容会根据关键词自动分类：

- **思维启发**：思维、认知、心理、哲学、逻辑、决策
- **好习惯**：习惯、健康、运动、睡眠、效率、时间管理
- **知识点**：科学、技术、历史、文化、教育、学习
- **热点趋势**：趋势、未来、创新、AI、区块链、startup

## 🎯 数据存储

### 数据库位置
```
~/.local/share/awake/awake.db
```

### 日志位置
```
~/.local/share/awake/logs/awake.log
```

### 数据库表结构

```sql
-- 内容表
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

-- 每日Feed表
CREATE TABLE daily_feeds (
    date TEXT PRIMARY KEY,
    content_ids TEXT NOT NULL,
    created_at INTEGER NOT NULL
);

-- 用户行为表
CREATE TABLE user_actions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    content_id TEXT NOT NULL,
    action_type TEXT NOT NULL,  -- 'favorite', 'skip', 'dislike'
    created_at INTEGER NOT NULL
);

-- 用户偏好表
CREATE TABLE user_preferences (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
);

-- 收藏表
CREATE TABLE favorites (
    content_id TEXT PRIMARY KEY,
    created_at INTEGER NOT NULL
);
```

## 🔧 配置选项

当前版本通过数据库 `user_preferences` 表配置：

- `thinking_count`: 思维启发内容数量（默认3）
- `habit_count`: 好习惯内容数量（默认2）
- `knowledge_count`: 知识点内容数量（默认3）
- `trend_count`: 趋势内容数量（默认2）
- `refresh_time`: 每日刷新时间（默认"08:00"）
- `auto_start`: 开机自启（默认false）
- `minimize_to_tray`: 最小化到托盘（默认true）
- `theme_mode`: 主题模式（默认"light"）

## 📈 未来扩展

项目架构已预留以下扩展点：

### 1. LLM内容生成

```cpp
class LLMContentProvider : public IContentProvider {
    // 使用Claude/GPT API生成每日内容
    // 或对RSS内容进行摘要和分类
};
```

### 2. RSS订阅源

```cpp
class RSSContentProvider : public IContentProvider {
    // 抓取RSS feeds
    // 使用LLM进行摘要和分类
};
```

### 3. 混合内容策略

```cpp
ContentProviderManager manager;
manager.addProvider(std::make_shared<SeedContentProvider>(seedPath));
manager.addProvider(std::make_shared<LLMContentProvider>(apiKey));
manager.addProvider(std::make_shared<RSSContentProvider>(feedUrls));
```

### 4. GUI界面

当前版本是命令行版本，未来可添加：
- Qt Widgets桌面GUI
- 系统托盘图标
- 设置对话框
- 内容卡片展示

## 🐛 调试

### 查看日志

```bash
tail -f ~/.local/share/awake/logs/awake.log
```

### 清空数据库重新开始

```bash
rm ~/.local/share/awake/awake.db
./build/awake
```

### 测试内容生成

修改 `main.cpp` 中的代码手动触发生成：

```cpp
auto feed = feedGenerator->generateDailyFeed(profile);
printFeed(feed);
```

## ❓ 常见问题

### Q: 编译时找不到Qt6？
A: 确保安装了qt6-base-dev，可以用 `apt list --installed | grep qt6` 检查。

### Q: 运行时找不到种子内容文件？
A: 确保 `resources/seed_content.json` 存在，并且在运行目录的相对路径下。

### Q: 如何修改刷新时间？
A: 目前需要直接修改数据库，或等待GUI设置界面。

### Q: 内容会重复吗？
A: 不会。系统会记录所有已展示的内容ID，不会重复推荐。

## 📝 开发计划

- [ ] 创建完整的Qt GUI界面
- [ ] 实现设置对话框
- [ ] 添加系统托盘功能
- [ ] 实现LLM内容生成
- [ ] 添加RSS订阅功能
- [ ] 支持导入/导出配置
- [ ] 跨设备同步（WebDAV）

## 📄 许可证

MIT

## 🤝 贡献

欢迎提交Issue和Pull Request！

## 作者

Fengze Han
