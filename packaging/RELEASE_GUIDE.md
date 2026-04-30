# Awake Package Release Guide

本指南介绍如何将 Awake 发布到 Homebrew 和 APT (Ubuntu PPA) 包管理器。

## 📋 前置准备

### 1. 创建 GitHub Release

```bash
# 1. 确保所有更改已提交
git status

# 2. 创建并推送版本标签
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0

# 3. 创建 GitHub Release
gh release create v1.0.0 \
  --title "Awake v1.0.0" \
  --notes "Initial public release

Features:
- Smart content generation with scoring algorithm
- User preference learning
- Auto daily refresh
- Qt5 GUI with system tray support
- RSS subscription support
- Cross-platform notification sounds"
```

### 2. 计算 tarball SHA256

```bash
# 下载 release tarball 并计算 SHA256
wget https://github.com/Fzzzhan/awake/archive/refs/tags/v1.0.0.tar.gz
sha256sum v1.0.0.tar.gz
```

## 🍺 Homebrew 发布流程

### 方案 A: 创建自己的 Tap (推荐新项目)

1. **创建 Homebrew Tap 仓库**

```bash
# 在 GitHub 上创建新仓库: homebrew-tap
gh repo create homebrew-tap --public --description "Homebrew tap for Awake"
```

2. **更新 Formula 并推送**

```bash
# 克隆 tap 仓库
git clone https://github.com/Fzzzhan/homebrew-tap.git
cd homebrew-tap

# 复制 formula
cp ../awake/packaging/awake.rb Formula/awake.rb

# 编辑 Formula，填入正确的 SHA256
nano Formula/awake.rb
# 将 sha256 "" 改为实际的 SHA256 值

# 提交并推送
git add Formula/awake.rb
git commit -m "Add awake formula v1.0.0"
git push origin main
```

3. **测试安装**

```bash
# 用户安装命令
brew tap Fzzzhan/tap
brew install awake

# 或一行安装
brew install Fzzzhan/tap/awake
```

### 方案 B: 提交到官方 Homebrew-core (适合成熟项目)

官方 homebrew-core 有严格要求：
- 项目必须有一定知名度（通常 75+ GitHub stars）
- 必须有稳定的版本发布历史
- 项目必须活跃维护

提交流程：
```bash
# Fork homebrew-core
gh repo fork homebrew/homebrew-core

# 创建 formula
cd homebrew-core
brew create https://github.com/Fzzzhan/awake/archive/refs/tags/v1.0.0.tar.gz

# 测试 formula
brew install --build-from-source ./Formula/awake.rb
brew test awake
brew audit --new-formula awake

# 提交 PR
git add Formula/awake.rb
git commit -m "awake 1.0.0 (new formula)"
git push origin main
gh pr create --web
```

## 📦 APT/PPA 发布流程 (Ubuntu Launchpad)

### 1. 注册 Launchpad 账号

1. 访问 https://launchpad.net/
2. 注册账号并验证邮箱
3. 上传 GPG 密钥

### 2. 生成 GPG 密钥

```bash
# 生成密钥
gpg --full-generate-key
# 选择: RSA and RSA, 4096 bits, no expiration
# 输入你的名字和邮箱 (必须与 Launchpad 账号匹配)

# 列出密钥
gpg --list-keys
# 记下 key ID (类似: 1234567890ABCDEF)

# 导出公钥并上传到 Ubuntu 密钥服务器
gpg --keyserver keyserver.ubuntu.com --send-keys YOUR_KEY_ID

# 复制公钥指纹到 Launchpad profile
gpg --fingerprint YOUR_KEY_ID
```

### 3. 创建 PPA

```bash
# 在 Launchpad 上创建 PPA
# 访问: https://launchpad.net/~your-username/+activate-ppa
# PPA 名称: awake
# 显示名称: Awake - Daily Cognitive Supplement
# 描述: Daily cognitive supplement tool with smart content generation
```

### 4. 准备源码包

```bash
cd /home/fengze/Github/morning_call

# 复制 debian 目录到项目根目录
cp -r packaging/debian .

# 确保 debian/changelog 中的邮箱与 GPG 密钥匹配
nano debian/changelog

# 构建源码包
debuild -S -sa

# 这会在上级目录生成:
# - awake_1.0.0-1.dsc
# - awake_1.0.0-1.debian.tar.xz
# - awake_1.0.0.orig.tar.gz
# - awake_1.0.0-1_source.changes
```

### 5. 上传到 PPA

```bash
# 安装 dput
sudo apt install dput

# 配置 dput
cat >> ~/.dput.cf << 'EOF'
[ppa]
fqdn = ppa.launchpad.net
method = ftp
incoming = ~your-launchpad-id/ubuntu/awake/
login = anonymous
allow_unsigned_uploads = 0
EOF

# 上传源码包
cd ..
dput ppa awake_1.0.0-1_source.changes

# Launchpad 会自动构建包 (需要等待 5-30 分钟)
```

### 6. 支持多个 Ubuntu 版本

为不同 Ubuntu 版本创建包：

```bash
# 为 Ubuntu 22.04 (jammy)
dch -v 1.0.0-1~jammy1 -D jammy "Backport to jammy"
debuild -S -sa
dput ppa awake_1.0.0-1~jammy1_source.changes

# 为 Ubuntu 20.04 (focal)
dch -v 1.0.0-1~focal1 -D focal "Backport to focal"
debuild -S -sa
dput ppa awake_1.0.0-1~focal1_source.changes
```

### 7. 用户安装命令

PPA 发布后，用户可以这样安装：

```bash
sudo add-apt-repository ppa:your-launchpad-id/awake
sudo apt update
sudo apt install awake
```

## 📊 发布检查清单

发布前检查：

- [ ] 版本号已更新 (CMakeLists.txt, debian/changelog)
- [ ] README 已更新
- [ ] 所有测试通过
- [ ] 文档完整
- [ ] LICENSE 文件存在
- [ ] CHANGELOG 已更新
- [ ] Git tag 已创建
- [ ] GitHub Release 已创建

发布后验证：

- [ ] Homebrew formula 可安装
- [ ] PPA 包可安装
- [ ] 安装后程序可正常运行
- [ ] Desktop 文件正确显示
- [ ] 图标正确显示
- [ ] 数据目录自动创建

## 🔄 更新版本流程

```bash
# 1. 更新版本号
# 编辑 CMakeLists.txt
sed -i 's/VERSION 1.0.0/VERSION 1.0.1/' CMakeLists.txt

# 更新 debian/changelog
dch -v 1.0.1-1 "New release"

# 2. 提交更改
git add .
git commit -m "Bump version to 1.0.1"
git push

# 3. 创建新标签
git tag -a v1.0.1 -m "Release v1.0.1"
git push origin v1.0.1

# 4. 重复发布流程...
```

## 📚 参考资源

- [Homebrew Formula Cookbook](https://docs.brew.sh/Formula-Cookbook)
- [Debian Packaging Guide](https://www.debian.org/doc/manuals/maint-guide/)
- [Launchpad PPA Guide](https://help.launchpad.net/Packaging/PPA)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)

## ❓ 常见问题

### Homebrew formula 测试失败
```bash
# 详细测试
brew install --build-from-source --verbose awake
brew test --verbose awake
```

### PPA 构建失败
- 检查 Launchpad 构建日志
- 确保依赖包名称正确
- 验证 debian/rules 文件权限 (必须可执行)

### GPG 签名失败
```bash
# 确保 GPG 密钥可用
gpg --list-secret-keys

# 重新配置 GPG agent
echo "use-agent" >> ~/.gnupg/gpg.conf
```
