pkg_search_module(MIR REQUIRED mirclient)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Quick REQUIRED)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/plugins/brightness
    ${CMAKE_SOURCE_DIR}/plugins/brightness/displays
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${MIR_INCLUDE_DIRS}
)

set(MOCK_BRIGHTNESS_SRCS
    MockAethercastDevice.h
    MockAethercastDeviceModel.cpp
    MockAethercastDisplays.cpp
    MockBrightness.cpp

    MockDisplay.h
    MockDisplayModel.h
    plugin.cpp
)

add_library(MockBrightnessPanel MODULE ${MOCK_BRIGHTNESS_SRCS})

add_dependencies(MockBrightnessPanel uss-displays)
target_link_libraries(MockBrightnessPanel uss-displays Qt5::Core Qt5::Quick ${Qt5Gui_PRIVATE_LDFLAGS} ${MIR_LDFLAGS})

add_uss_mock(Ubuntu.SystemSettings.Brightness 1.0 Ubuntu/SystemSettings/Brightness
             TARGETS MockBrightnessPanel)
