#include "breezex11.h"

#if BREEZE_HAVE_X11

namespace Breeze
{

xcb_window_t breezeX11RootWindow(xcb_connection_t *connection, int defaultScreen)
{
    if (!connection || defaultScreen < 0) return XCB_WINDOW_NONE;

    const xcb_setup_t *setup = xcb_get_setup(connection);
    if (!setup) return XCB_WINDOW_NONE;

    xcb_screen_iterator_t screens = xcb_setup_roots_iterator(setup);
    for (int index = 0; screens.rem && screens.data; ++index, xcb_screen_next(&screens)) {
        if (index == defaultScreen) return screens.data->root;
    }

    return XCB_WINDOW_NONE;
}

}

#endif
