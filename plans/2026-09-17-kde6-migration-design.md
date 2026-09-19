# Breeze10 KDE6 Migration Design

**Date:** 2026-09-17  
**Status:** Approved design; implementation plan follows after review  
**Target:** Plasma 6.0 and KF6 6.0 minimum, with localized support for later KDecoration API variants

## 1. Goals and non-goals

Migrate Breeze10 from Qt 5/KF5/KDecoration2-era build and plugin integration to Qt 6/KF6 and the Plasma 6 decoration architecture while preserving the existing user-visible behavior:

- Configurable title-bar opacity and opacity overrides.
- Exception-list matching and per-window settings.
- Independent title-bar font behavior.
- Existing borders, buttons, shadows, animations, maximized-window behavior, and size-grip behavior.
- X11-only window detection and size grip, with safe no-op behavior on Wayland.
- The same settings exposed through the Plasma 6 Window Decorations KCM.

The migration does not redesign the UI, change the settings schema, modernize every legacy signal/slot connection, or add dual Qt5/KF5 support.

## 2. Chosen approach

Use a targeted, upstream-aligned port rather than a broad rebase or mechanical in-place conversion. Follow the Plasma 6.0 upstream Breeze structure for plugin discovery and KCM loading, while retaining Breeze10-specific source and behavior.

The key architectural boundary is:

1. `breeze10common6`: shared rendering/common code from [`libbreezecommon/CMakeLists.txt`](../libbreezecommon/CMakeLists.txt:1).
2. A decoration plugin containing runtime decoration sources and the decoration factory from [`breezedecoration.cpp`](../breezedecoration.cpp:54).
3. A separate KCM plugin under [`config/`](../config/), containing the existing configuration widgets and a small Plasma 6-compatible KCM entry point.

The KCM split is a loading/integration boundary only. Both plugins continue to use the same generated settings classes and configuration schema.

## 3. Build-system and dependency design

Update [`CMakeLists.txt`](../CMakeLists.txt:1) and [`libbreezecommon/CMakeLists.txt`](../libbreezecommon/CMakeLists.txt:1) to use Qt 6, ECM/KF6, and KDE install conventions.

- Raise the CMake/ECM requirements to versions compatible with Plasma 6/KF6 6.0.
- Replace Qt5 and KF5 package names and link targets with Qt6/KF6 equivalents.
- Add KCMUtils for the dedicated KCM target.
- Keep WindowSystem for [`KWindowInfo`](../breezesettingsprovider.cpp:25) and `NET::` property access.
- Keep XCB optional. When XCB is found, compile the X11-specific code and link XCB; otherwise disable only X11-specific features.
- Remove the Qt5 X11Extras dependency. Qt 6 native X11 access is provided through `Qt6::Gui`.
- Rename the common library to `breeze10common6` and update all consumers and generated export-header references.
- Detect the installed KDecoration package and select a single internal target/compile definition for the supported KDecoration2 Plasma 6.0 baseline and later KDecoration3-style packages. Keep this compatibility logic in CMake and small API adapters, not duplicated source trees.
- Use the KDecoration-provided plugin-directory variables. Do not hard-code the old `org.kde.kdecoration2` install path.
- Build the decoration and KCM as independent targets. Use `kcoreaddons_add_plugin` for the KCM and `kcmutils_generate_desktop_file` to generate discoverable metadata.
- Remove the obsolete hand-authored KCM service file only after the staged installation confirms the generated metadata and plugin path.

The decoration JSON metadata remains associated with the decoration plugin. Its service type and KCM indication must be validated against the selected Plasma 6 KDecoration package rather than assumed to be unchanged.

## 4. Source and API design

### Client access

The KDecoration API changes the client accessor shape between supported variants. Replace `client().data()` only where `client()` returns a raw `DecoratedClient*`; preserve `.data()` on unrelated Qt smart pointers such as `settings()`.

Use a small compatibility accessor if required so source files do not accumulate version conditionals. Apply this consistently in [`breezedecoration.cpp`](../breezedecoration.cpp:194), [`breezedecoration.h`](../breezedecoration.h:182), [`breezebutton.cpp`](../breezebutton.cpp:57), [`breezesettingsprovider.cpp`](../breezesettingsprovider.cpp:78), and [`breezesizegrip.cpp`](../breezesizegrip.cpp:65). Preserve null checks at plugin/object boundaries.

### X11 access

Remove `QX11Info` includes and calls. For Qt 6, obtain `QNativeInterface::QX11Application` from the application object and check for null before access. Use its XCB connection for existing operations.

Because the old `appRootWindow()` convenience is not available through the same API, derive the root window from the XCB connection/setup/screen data in a small local helper or equivalent shared implementation. Use that root for the synthetic release and `_NET_WM_MOVERESIZE` messages.

Apply this to [`breezedecoration.cpp`](../breezedecoration.cpp:48), [`breezesizegrip.cpp`](../breezesizegrip.cpp:29), [`config/breezedetectwidget.cpp`](../config/breezedetectwidget.cpp:39), and [`config/breezeexceptiondialog.cpp`](../config/breezeexceptiondialog.cpp:30). The runtime rule is unchanged: X11 operations run only when both XCB support was compiled and the Qt platform exposes an X11 native interface.

### Size grip and window detection

Preserve the existing X11 size-grip sequence: embedding/reparenting, stacking above the client, geometry updates, synthetic button release, and `_NET_WM_MOVERESIZE` dispatch. On Wayland or a no-XCB build, skip these operations safely.

Preserve the existing detect-window UI behavior: hide or disable the X11-only control when no X11 native interface is available, and avoid dereferencing an absent connection.

### Window-system and regular-expression APIs

Continue using KF6 WindowSystem’s `KWindowInfo` and `NET::` values, validating the actual KF6 constructor signatures during compilation. Replace removed Qt5 `QRegExp` usage in [`breezesettingsprovider.cpp`](../breezesettingsprovider.cpp:130) and [`config/breezeexceptionlistwidget.cpp`](../config/breezeexceptionlistwidget.cpp:326) with `QRegularExpression`, preserving the existing validity checks and pattern matching semantics.

Convert the client icon connection in [`breezebutton.cpp`](../breezebutton.cpp:57) to typed Qt syntax. Leave other old-style widget connections unchanged unless the Qt6 build demonstrates that a particular connection is invalid.

## 5. KCM architecture and metadata

Move the existing configuration source set and generated UI forms into a dedicated KCM target under [`config/`](../config/). Add the minimal Plasma 6 KCM factory/entry point required by `kcoreaddons_add_plugin` while retaining the existing widget classes and settings behavior.

Link the KCM against Qt6 Core/Gui/DBus and the KF6 components required by the current implementation, including ConfigCore, CoreAddons, GuiAddons, I18n, IconThemes, and KCMUtils. Link the decoration target only against its runtime requirements.

Generate KCM desktop metadata through the KF6 KCMUtils CMake helper and install it in the KDecoration KCM plugin namespace. Do not use `kcmshell5` or the old `X-KDE-Library` hand-authored service file as the primary discovery mechanism.

Validate that [`breeze.json`](../breeze.json:1) is accepted by the selected Plasma 6 KDecoration package, that the decoration is discoverable, and that the decoration points to the separately installed KCM.

## 6. Validation and failure handling

### Build checkpoints

1. Configure a clean build with Qt6/KF6 6.0 dependencies and testing enabled where supported.
2. Build after the target split before broad source cleanup, so dependency and factory errors are isolated.
3. Build with XCB present and absent when environments permit.
4. Build against the Plasma 6.0/KDecoration2 baseline; build against a later supported KDecoration package when available.
5. Install to a staging prefix and inspect the decoration module, common library, KCM module, generated desktop metadata, JSON metadata, and settings files.

Required dependencies should produce clear configure failures. Missing optional XCB must not prevent a non-X11 build.

### Runtime acceptance

- KWin loads Breeze10 without plugin-loader or unresolved-symbol errors.
- Window Decorations lists Breeze10 and opens the separate KCM.
- Existing settings persist and affect rendering, including opacity, exceptions, font, borders, buttons, shadows, animations, and maximized-window behavior.
- X11 window detection works and the size grip retains its previous behavior.
- Wayland remains crash-free; X11-only operations and controls are skipped or hidden.

Failure handling is conservative: null-check clients and native interfaces at boundaries, preserve optional-XCB behavior, and fail loudly for missing required KDE components rather than silently masking an invalid installation.

## 7. Documentation and release notes

Update [`README.md`](../README.md:16) to describe Qt6/KF6/Plasma 6 dependencies, the current build/install commands, separate KCM integration, and optional X11 support. Remove Qt5/KF5 package examples and `kcmshell5` assumptions.

Update [`NEWS`](../NEWS:1) and/or [`ChangeLog`](../ChangeLog:1) to state that this release is KDE6/Plasma 6-only and is not ABI- or dependency-compatible with the KDE5 build. Document the renamed common library only if it is user/package-visible.

## 8. Out of scope

- Visual redesign or settings-schema changes.
- Dual KDE5/KDE6 compatibility.
- Broad rebase onto upstream Breeze.
- Complete modernization of all signal/slot syntax.
- Removal of the X11 size grip.
- New automated desktop integration infrastructure beyond build, staged-install, and available KCM smoke validation.

## 9. Completion criteria

The migration is complete when the clean Qt6/KF6 build succeeds for the selected baseline, the staged install contains independently discoverable decoration and KCM plugins, X11 and Wayland paths satisfy the runtime acceptance criteria, and the documentation/release notes accurately describe the KDE6-only result.
