set(breeze10_kcm_SRCS
    breezeconfigwidget.cpp
    breezedetectwidget.cpp
    breezeexceptiondialog.cpp
    breezeexceptionlistwidget.cpp
    breezeexceptionmodel.cpp
    breezeitemmodel.cpp
    kcm_breeze10.cpp
    ../breezeexceptionlist.cpp
    ../breezex11.cpp)

set(breeze10_kcm_FORMS
    ui/breezeconfigurationui.ui
    ui/breezedetectwidget.ui
    ui/breezeexceptiondialog.ui
    ui/breezeexceptionlistwidget.ui)

ki18n_wrap_ui(breeze10_kcm_FORMS_HEADERS ${breeze10_kcm_FORMS})
kconfig_add_kcfg_files(breeze10_kcm_SRCS ../breezesettings.kcfgc)

kcoreaddons_add_plugin(kcm_breeze10
    SOURCES ${breeze10_kcm_SRCS} ${breeze10_kcm_FORMS_HEADERS}
    INSTALL_NAMESPACE "${KDECORATION_KCM_PLUGIN_DIR}")

kcmutils_generate_desktop_file(kcm_breeze10)

target_include_directories(kcm_breeze10 PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(kcm_breeze10
    PRIVATE Qt6::Core Qt6::Gui Qt6::DBus Qt6::Widgets
        KF6::ConfigCore KF6::ConfigWidgets KF6::CoreAddons KF6::GuiAddons KF6::I18n
        KF6::IconThemes KF6::KCMUtils)
if(BREEZE_HAVE_X11)
    target_link_libraries(kcm_breeze10 PRIVATE XCB::XCB)
endif()
