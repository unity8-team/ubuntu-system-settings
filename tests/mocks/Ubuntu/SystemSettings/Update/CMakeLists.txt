include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/system-update
)

set(MOCK_SYSTEMUPDATE_SRCS
    plugin.cpp
    MockClickUpdateManager.cpp
    MockSystemImage.cpp
    MockUpdateModel.h
    ${CMAKE_SOURCE_DIR}/plugins/system-update/enums.h
)

add_library(MockSystemUpdate MODULE ${MOCK_SYSTEMUPDATE_SRCS})

qt5_use_modules(MockSystemUpdate Qml Quick Core)

add_uss_mock(Ubuntu.SystemSettings.Update 1.0 Ubuntu/SystemSettings/Update
             TARGETS MockSystemUpdate)
