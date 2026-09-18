#include "breezex11.h"

#if BREEZE_HAVE_X11

namespace Breeze
{

xcb_window_t breezeX11RootWindow(xcb_connection_t *connection)
{
    if (!connection) return XCB_WINDOW_NONE;

    const xcb_setup_t *setup = xcb_get_setup(connection);
    if (!setup) return XCB_WINDOW_NONE;

    xcb_screen_iterator_t screens = xcb_setup_roots_iterator(setup);
    if (!screens.data) return XCB_WINDOW_NONE;

    return screens.data->root;
}

}

#endif
