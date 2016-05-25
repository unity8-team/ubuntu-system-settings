include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(MOCK_DOWNLOADMANAGER_SRCS
    plugin.cpp
    MockMetadata.cpp
    MockSingleDownload.cpp
    MockDownloadManager.cpp
)

add_library(MockUbuntuDownloadManager MODULE ${MOCK_DOWNLOADMANAGER_SRCS})

qt5_use_modules(MockUbuntuDownloadManager Qml Quick Core)

add_uss_mock(Ubuntu.DownloadManager 1.2 Ubuntu.SystemSettings.Update
             TARGETS MockUbuntuDownloadManager)
