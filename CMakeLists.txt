cmake_minimum_required(VERSION 3.16)
project(awake VERSION 1.0.0 LANGUAGES CXX)

# Suppress RPATH warnings when using anaconda Qt5
cmake_policy(SET CMP0042 NEW)
if(POLICY CMP0068)
    cmake_policy(SET CMP0068 NEW)
endif()

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compile options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Fix RPATH to avoid conflicts with anaconda
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
# Prefer system libraries over anaconda
set(CMAKE_BUILD_RPATH "/usr/lib/x86_64-linux-gnu;/usr/lib;/usr/local/lib")
set(CMAKE_INSTALL_RPATH "/usr/lib/x86_64-linux-gnu;/usr/lib;/usr/local/lib")
# Don't add Qt5's library paths to RPATH
set(CMAKE_NO_SYSTEM_FROM_IMPORTED TRUE)

# Include CPM package manager
include(cmake/CPM.cmake)

# ===== CPM Dependencies =====

# nlohmann_json
CPMAddPackage(
    NAME nlohmann_json
    VERSION 3.11.3
    GITHUB_REPOSITORY nlohmann/json
    OPTIONS
        "JSON_BuildTests OFF"
        "JSON_Install OFF"
)

# spdlog
CPMAddPackage(
    NAME spdlog
    VERSION 1.13.0
    GITHUB_REPOSITORY gabime/spdlog
    OPTIONS
        "SPDLOG_BUILD_EXAMPLE OFF"
        "SPDLOG_BUILD_TESTS OFF"
        "SPDLOG_INSTALL OFF"
)

# SQLite3 - Download amalgamation for headers
CPMAddPackage(
    NAME sqlite3
    VERSION 3.45.0
    URL https://www.sqlite.org/2024/sqlite-amalgamation-3450000.zip
    DOWNLOAD_ONLY YES
)

# Set SQLite3 include directory from CPM download
if(sqlite3_ADDED)
    set(SQLITE3_INCLUDE_DIRS ${sqlite3_SOURCE_DIR})
    message(STATUS "SQLite3 headers from CPM: ${SQLITE3_INCLUDE_DIRS}")
endif()

# Find system SQLite3 library
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(SQLITE3_LIB sqlite3)
endif()

# If not found via pkg-config, use direct library path
if(NOT SQLITE3_LIB_FOUND)
    if(EXISTS "/usr/lib/x86_64-linux-gnu/libsqlite3.so.0")
        set(SQLITE3_LIBRARIES /usr/lib/x86_64-linux-gnu/libsqlite3.so.0)
        message(STATUS "Using system SQLite3 library: ${SQLITE3_LIBRARIES}")
    else()
        message(FATAL_ERROR "SQLite3 library not found. Please install: sudo apt install libsqlite3-0")
    endif()
else()
    set(SQLITE3_LIBRARIES ${SQLITE3_LIB_LIBRARIES})
endif()

# ===== Qt5 Dependencies (system) =====
# Qt5 is too large for CPM, use system installation

# Prioritize system library paths before anaconda
link_directories(BEFORE /usr/lib/x86_64-linux-gnu /usr/lib /usr/local/lib)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network)

# Note if using anaconda Qt5
if(Qt5_DIR MATCHES "anaconda")
    message(STATUS "Using Qt5 from anaconda: ${Qt5_DIR}")

    # Remove anaconda lib paths from Qt5 targets to suppress RPATH warnings
    foreach(qt_target Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
        get_target_property(qt_location ${qt_target} LOCATION)
        if(qt_location)
            get_filename_component(qt_lib_dir ${qt_location} DIRECTORY)
            set_property(TARGET ${qt_target} PROPERTY INTERFACE_LINK_DIRECTORIES "")
            message(STATUS "Cleared INTERFACE_LINK_DIRECTORIES for ${qt_target}")
        endif()
    endforeach()
endif()

# Enable Qt MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ===== Source Files =====

# Common source files
set(COMMON_SOURCES
    src/core/FeedGenerator.cpp
    src/core/ContentScorer.cpp
    src/core/Scheduler.cpp
    src/data/Database.cpp
    src/data/ContentRepository.cpp
    src/data/UserProfileRepository.cpp
    src/services/ContentProvider.cpp
    src/services/PreferenceManager.cpp
    src/services/SeedContentLoader.cpp
    src/services/RSSParser.cpp
    src/services/RSSFeedConfig.cpp
    src/models/ContentItem.cpp
    src/models/UserProfile.cpp
    src/models/DailyFeed.cpp
    src/utils/Logger.cpp
    src/utils/JsonHelper.cpp
)

# Console version sources
set(CONSOLE_SOURCES
    src/main.cpp
)

# GUI version sources
set(GUI_SOURCES
    src/main_gui.cpp
    src/ui/MainWindow.cpp
    src/ui/ContentCard.cpp
    src/ui/SettingsDialog.cpp
)

set(COMMON_HEADERS
    src/core/FeedGenerator.h
    src/core/ContentScorer.h
    src/core/Scheduler.h
    src/data/Database.h
    src/data/ContentRepository.h
    src/data/UserProfileRepository.h
    src/services/ContentProvider.h
    src/services/PreferenceManager.h
    src/services/SeedContentLoader.h
    src/models/ContentItem.h
    src/models/UserProfile.h
    src/models/DailyFeed.h
    src/utils/Logger.h
    src/utils/JsonHelper.h
)

set(GUI_HEADERS
    src/ui/MainWindow.h
    src/ui/ContentCard.h
    src/ui/SettingsDialog.h
)

# ===== Executables =====

# Create console executable
add_executable(${PROJECT_NAME} ${CONSOLE_SOURCES} ${COMMON_SOURCES} ${COMMON_HEADERS})

# Create GUI executable
add_executable(${PROJECT_NAME}_gui ${GUI_SOURCES} ${COMMON_SOURCES} ${COMMON_HEADERS} ${GUI_HEADERS})

# ===== Link Libraries =====

# Link libraries for console version
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    ${SQLITE3_LIBRARIES}
)

# Set RPATH for console version - use BUILD_WITH_INSTALL_RPATH to skip CMake's RPATH check
set_target_properties(${PROJECT_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "/usr/lib/x86_64-linux-gnu;/usr/lib;\$ORIGIN"
)

# Link libraries for GUI version
target_link_libraries(${PROJECT_NAME}_gui PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    ${SQLITE3_LIBRARIES}
)

# Set RPATH for GUI version - use BUILD_WITH_INSTALL_RPATH to skip CMake's RPATH check
set_target_properties(${PROJECT_NAME}_gui PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "/usr/lib/x86_64-linux-gnu;/usr/lib;\$ORIGIN"
)

# ===== Include Directories =====

# Include directories for console version
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SQLITE3_INCLUDE_DIRS}
)

# Include directories for GUI version
target_include_directories(${PROJECT_NAME}_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SQLITE3_INCLUDE_DIRS}
)

# ===== Installation =====

# Install targets
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_gui
    RUNTIME DESTINATION bin
)

# Install .desktop file
install(FILES resources/awake.desktop
    DESTINATION share/applications
)

# Install icon
install(FILES resources/icons/awake.png
    DESTINATION share/icons/hicolor/256x256/apps
)

# ===== Summary =====

message(STATUS "==============================================")
message(STATUS "Awake Configuration Summary")
message(STATUS "==============================================")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt5 Version: ${Qt5_VERSION}")
message(STATUS "Qt5 Location: ${Qt5_DIR}")
message(STATUS "spdlog: ${spdlog_VERSION} (CPM)")
message(STATUS "nlohmann_json: ${nlohmann_json_VERSION} (CPM)")
message(STATUS "SQLite3: ${SQLITE3_LIBRARIES}")
message(STATUS "==============================================")
