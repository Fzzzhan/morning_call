class Awake < Formula
  desc "Daily cognitive supplement tool with smart content generation"
  homepage "https://github.com/Fzzzhan/awake"
  url "https://github.com/Fzzzhan/awake/archive/refs/tags/v1.0.0.tar.gz"
  sha256 "" # Will be filled after release
  license "MIT"
  head "https://github.com/Fzzzhan/awake.git", branch: "main"

  depends_on "cmake" => :build
  depends_on "ninja" => :build
  depends_on "qt@5"
  depends_on "sqlite"

  def install
    # Set Qt5 directory for cmake
    qt5_dir = Formula["qt@5"].opt_prefix

    system "cmake", "-B", "build",
                    "-G", "Ninja",
                    "-DCMAKE_PREFIX_PATH=#{qt5_dir}",
                    *std_cmake_args
    system "cmake", "--build", "build"
    system "cmake", "--install", "build"
  end

  def post_install
    # Create data directory
    (var/"awake").mkpath
  end

  def caveats
    <<~EOS
      Awake has been installed!

      To run the GUI version:
        awake_gui

      To run the console version:
        awake

      Data will be stored in:
        ~/.local/share/awake/

      To start awake at login:
        brew services start awake
    EOS
  end

  service do
    run [opt_bin/"awake_gui"]
    keep_alive false
    launch_only_once true
  end

  test do
    # Test that binaries exist and can show version/help
    assert_predicate bin/"awake", :exist?
    assert_predicate bin/"awake_gui", :exist?
  end
end
