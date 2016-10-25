include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/brightness/
    ${CMAKE_SOURCE_DIR}/plugins/brightness/displays
)

set(MOCK_BRIGHTNESS_SRCS
    MockAethercastDevice.h
    MockAethercastDeviceModel.cpp
    MockAethercastDisplays.cpp
    MockBrightness.cpp

    ${CMAKE_SOURCE_DIR}/plugins/brightness/displays/display.h
    ${CMAKE_SOURCE_DIR}/plugins/brightness/displays/displaymodel.h
    MockDisplay.h
    MockDisplayModel.h
    ${CMAKE_SOURCE_DIR}/plugins/brightness/displays/display.cpp
    ${CMAKE_SOURCE_DIR}/plugins/brightness/displays/displaymodel.cpp
    plugin.cpp
)

add_library(MockBrightnessPanel MODULE ${MOCK_BRIGHTNESS_SRCS})

target_link_libraries(MockBrightnessPanel)
qt5_use_modules(MockBrightnessPanel Qml Quick Core)

add_uss_mock(Ubuntu.SystemSettings.Brightness 1.0 Ubuntu/SystemSettings/Brightness
             TARGETS MockBrightnessPanel)
