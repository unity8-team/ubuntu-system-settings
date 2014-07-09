set(BACKGROUND_IMAGES_DIR "${CMAKE_INSTALL_PREFIX}/share/ubuntu/settings/system/background_images")

set(BACKGROUND_IMAGES
    cakes.jpg
    launchpad.jpg
    redtrain.jpg
    spaceman.jpg
)

install(FILES ${BACKGROUND_IMAGES} DESTINATION ${BACKGROUND_IMAGES_DIR})
