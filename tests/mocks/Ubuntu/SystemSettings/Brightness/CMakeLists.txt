include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/brightness/
)

set(MOCK_BRIGHTNESS_SRCS
    MockAethercastDevice.h
    MockAethercastDeviceModel.cpp
    MockAethercastDisplays.cpp
    MockBrightness.cpp
    plugin.cpp
)

add_library(MockBrightnessPanel MODULE ${MOCK_BRIGHTNESS_SRCS})

target_link_libraries(MockBrightnessPanel ${GIO_LDFLAGS} ${QTGSETTINGS_LDFLAGS})
qt5_use_modules(MockBrightnessPanel Qml Quick Core)

add_uss_mock(Ubuntu.SystemSettings.Brightness 1.0 Ubuntu/SystemSettings/Brightness
             TARGETS MockBrightnessPanel)
