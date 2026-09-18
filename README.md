# Breeze10

## Overview

Breeze10 is a fork of KDE Breeze decoration with the following changes:

 * The title-bar opacity is configurable.
 * The separator between title-bar and window is removed.
 * Opaqueness, opacity override is added to the exception list properties.
 * Title-bar font is set indpendent from the KDE font settings (for use outside KDE).

## Credits

Breeze10 was started from BreezeEnhanced (https://github.com/tsujan/BreezeEnhanced), a former fork of Breeze with title-bar translucency and blurring.

## Compatibility

Breeze10 targets Plasma 6 and KF6 with Qt 6. Plasma 5, KDE Frameworks 5, and Qt 5 are not supported.

The build requires CMake, Extra CMake Modules, Qt 6, KF6 components (including KConfig, KConfigWidgets, KCoreAddons, KGuiAddons, KWindowSystem, KI18n, KIconThemes, and KCMUtils), and the Plasma 6 KDecoration development package. XCB is optional and enables the X11-only native window features.

## Installation

Configure, build, and install from the source directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build --parallel
sudo cmake --install build
```

Breeze10 is installed as a Plasma 6 window-decoration plugin. Its configuration module is a separate KCM plugin and appears in *System Settings -> Application Style -> Window Decorations* after KWin is restarted or the session is reloaded.

X11 window detection and the size-grip XCB operations are available only when XCB is found and the application is running with a native X11 interface. On Wayland, those paths are skipped and the detection control is unavailable.

## Known Issues

The KDE6 port has not been runtime-tested in this environment because the required KF6 and KDecoration development packages are unavailable.

## Screenshots

![Settings](screenshots/Settings.png?raw=true "Settings")

![Desktop](screenshots/Desktop.png?raw=true "Desktop")
