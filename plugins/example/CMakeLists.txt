include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(example-plugin MODULE example-plugin.cpp example-plugin.h)
qt5_use_modules(example-plugin Qml Core)
target_link_libraries(example-plugin SystemSettings)
