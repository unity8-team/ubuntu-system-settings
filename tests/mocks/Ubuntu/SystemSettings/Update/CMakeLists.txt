include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(MOCK_SYSTEMUPDATE_SRCS
    plugin.cpp
    MockClickUpdateManager.cpp
    MockSystemImage.cpp
    MockSystemUpdate.cpp
    MockUpdateModel.h
)

add_library(MockSystemUpdate MODULE ${MOCK_SYSTEMUPDATE_SRCS})

qt5_use_modules(MockSystemUpdate Qml Quick Core)

add_uss_mock(Ubuntu.SystemSettings.Update 1.0 Ubuntu/SystemSettings/Update
             TARGETS MockSystemUpdate)
