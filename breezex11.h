#ifndef breezex11_h
#define breezex11_h

#include "config-breeze.h"

#if BREEZE_HAVE_X11
#include <xcb/xcb.h>

namespace Breeze
{

xcb_window_t breezeX11RootWindow();

}
#endif

#endif
