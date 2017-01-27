include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Quick_INCLUDE_DIRS}
)

set(Qtlabs_folderlistmodel_SRCS
  MockFolderListModel.cpp
  plugin.cpp
)

add_library(QtLabsFolderListModelQml MODULE ${Qtlabs_folderlistmodel_SRCS})
target_link_libraries(QtLabsFolderListModelQml
  ${Qt5Core_LIBRARIES}
  ${Qt5Quick_LIBRARIES}
)

qt5_use_modules(QtLabsFolderListModelQml Qml)

add_uss_mock(Qt.labs.folderlistmodel 2.1 Qt.labs.folderlistmodel TARGETS QtLabsFolderListModelQml)
