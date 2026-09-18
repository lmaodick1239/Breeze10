#include "breezex11.h"

#if BREEZE_HAVE_X11

#include <QGuiApplication>
#include <QtGui/qnativeinterface.h>
#include <X11/Xlib.h>

namespace Breeze
{

xcb_window_t breezeX11RootWindow()
{
    if (!qGuiApp) return XCB_WINDOW_NONE;

    const auto *x11 = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if (!x11 || !x11->display() || !x11->connection()) return XCB_WINDOW_NONE;

    const int defaultScreen = DefaultScreen(x11->display());
    const xcb_setup_t *setup = xcb_get_setup(x11->connection());
    if (!setup || defaultScreen < 0) return XCB_WINDOW_NONE;

    xcb_screen_iterator_t screens = xcb_setup_roots_iterator(setup);
    for (int index = 0; screens.rem && screens.data; ++index, xcb_screen_next(&screens)) {
        if (index == defaultScreen) return screens.data->root;
    }

    return XCB_WINDOW_NONE;
}

}

#endif
