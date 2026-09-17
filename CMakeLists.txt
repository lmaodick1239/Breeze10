cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(breeze10 VERSION 0.1)

include(WriteBasicConfigVersionFile)
include(FeatureSummary)

find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(GenerateExportHeader)
find_package(KF6 6.0 REQUIRED COMPONENTS CoreAddons GuiAddons ConfigWidgets WindowSystem I18n KCMUtils IconThemes ConfigGui)
find_package(Qt6 CONFIG REQUIRED COMPONENTS Core Gui DBus Widgets)

find_package(KDecoration2 6.0 CONFIG QUIET)
if(KDecoration2_FOUND)
    set(BREEZE_KDECORATION_TARGET KDecoration2::KDecoration)
else()
    find_package(KDecoration3 6.0 CONFIG QUIET)
    if(KDecoration3_FOUND)
        set(BREEZE10_HAVE_KDECORATION3 TRUE)
        set(BREEZE_KDECORATION_TARGET KDecoration3::KDecoration)
    else()
        message(FATAL_ERROR "Neither KDecoration2 nor KDecoration3 was found")
    endif()
endif()

add_definitions(-DTRANSLATION_DOMAIN="breeze_kwin_deco")

find_package(XCB COMPONENTS XCB)
set_package_properties(XCB PROPERTIES
    DESCRIPTION "X protocol C-language Binding"
    URL "http://xcb.freedesktop.org"
    TYPE OPTIONAL
    PURPOSE "Required to pass style properties to native Windows on X11 Platform")

if(UNIX AND NOT APPLE)
    set(BREEZE_HAVE_X11 ${XCB_XCB_FOUND})
else()
    set(BREEZE_HAVE_X11 FALSE)
endif()

configure_file(config-breeze.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-breeze.h)

if(BREEZE10_HAVE_KDECORATION3)
    add_compile_definitions(BREEZE10_HAVE_KDECORATION3)
endif()

if(NOT DEFINED KDECORATION_PLUGIN_DIR)
    if(DEFINED KDE_INSTALL_PLUGINDIR)
        set(KDECORATION_PLUGIN_DIR "${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration2")
    else()
        set(KDECORATION_PLUGIN_DIR "${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration2")
    endif()
endif()
if(NOT DEFINED KDECORATION_KCM_PLUGIN_DIR)
    set(KDECORATION_KCM_PLUGIN_DIR "${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration2")
endif()

add_subdirectory(libbreezecommon)

set(breeze10_SRCS
    breezebutton.cpp
    breezedecoration.cpp
    breezeexceptionlist.cpp
    breezesettingsprovider.cpp
    breezesizegrip.cpp)
kconfig_add_kcfg_files(breeze10_SRCS breezesettings.kcfgc)

add_library(breeze10 MODULE ${breeze10_SRCS})
target_link_libraries(breeze10
    PUBLIC Qt6::Core Qt6::Gui Qt6::DBus
    PRIVATE breeze10common6 ${BREEZE_KDECORATION_TARGET}
        KF6::ConfigCore KF6::ConfigGui KF6::CoreAddons KF6::GuiAddons
        KF6::I18n KF6::IconThemes KF6::WindowSystem)
if(BREEZE_HAVE_X11)
    target_link_libraries(breeze10 PRIVATE XCB::XCB)
endif()
install(TARGETS breeze10 DESTINATION ${KDECORATION_PLUGIN_DIR})

add_subdirectory(config)
