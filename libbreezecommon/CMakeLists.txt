find_package(Qt6 REQUIRED CONFIG COMPONENTS Widgets)

configure_file(config-breezecommon.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-breezecommon.h)

set(breeze10common_LIB_SRCS
    breezeboxshadowrenderer.cpp)

add_library(breeze10common6 ${breeze10common_LIB_SRCS})
generate_export_header(breeze10common6
    BASE_NAME breezecommon
    EXPORT_FILE_NAME breezecommon_export.h)

target_link_libraries(breeze10common6
    PUBLIC Qt6::Core Qt6::Gui)

set_target_properties(breeze10common6 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS breeze10common6 ${INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
