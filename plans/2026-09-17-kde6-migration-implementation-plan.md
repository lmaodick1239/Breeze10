# Breeze10 KDE6 Migration Implementation Plan

> **For agentic workers:** Use a task-by-task implementation workflow with review checkpoints.

**Goal:** Port Breeze10 to Qt6/KF6 and Plasma 6 while preserving decoration behavior, the X11 size grip, and the configuration UI through a separate KCM plugin.

**Architecture:** Keep the existing decoration code and settings schema, split the KCM from the decoration module, and isolate KDecoration API differences in build configuration or small adapters. Follow the Plasma 6.0 KDecoration2 layout while allowing localized handling for later KDecoration variants.

**Tech Stack:** CMake, ECM, Qt6, KF6, KDecoration2/KDecoration3 compatibility, XCB, KDE KCMUtils.

## Global Constraints

- Plasma 6.0 and KF6 6.0 are the minimum supported baseline.
- KDE5/Qt5 compatibility is out of scope.
- XCB remains optional; X11-only features must be disabled safely without it.
- Preserve the existing settings schema and user-visible configuration behavior.
- Preserve the X11-only size grip and window-detection workflow.
- Do not broadly modernize unrelated signal/slot connections.
- Do not perform an unrelated visual redesign or broad upstream rebase.

---

### Stage 1: Build System and Target Decomposition

**Files:**
- Modify: [`CMakeLists.txt`](../CMakeLists.txt:1)
- Modify: [`libbreezecommon/CMakeLists.txt`](../libbreezecommon/CMakeLists.txt:1)
- Create: `config/kcm_breeze10.cpp`
- Create: `config/kcm_breeze10.json`
- Modify: [`breeze.json`](../breeze.json:1)
- Remove after staged metadata validation: [`config/breeze10config.desktop`](../config/breeze10config.desktop:1)
- Remove after staged metadata validation: [`config/breezedecorationconfig.desktop`](../config/breezedecorationconfig.desktop:1)

**Interfaces:**
- Produces shared target `breeze10common6`.
- Produces the decoration module target and its JSON metadata.
- Produces a separate KCM plugin target using `Breeze::ConfigWidget`.
- The KCM entry point must register `Breeze::ConfigWidget` with `K_PLUGIN_CLASS_WITH_JSON`.

- [ ] **Step 1: Record the current build baseline**

Run:

```sh
cmake -S . -B build-kde5-baseline -DCMAKE_BUILD_TYPE=Debug
```

Expected: The existing Qt5/KF5 project either configures successfully or reports the environment’s missing legacy dependencies. Preserve the result as diagnostic context; do not change source files for this baseline.

- [ ] **Step 2: Update root dependency declarations**

In [`CMakeLists.txt`](../CMakeLists.txt:6):

- Raise the minimum CMake/ECM versions to the agreed Plasma 6-compatible floors.
- Replace `KF5` package discovery with KF6 components, including `KCMUtils`.
- Replace `Qt5` package discovery and link targets with Qt6.
- Remove the `Qt5::X11Extras` package lookup.
- Keep XCB optional and retain `BREEZE_HAVE_X11` generation.
- Include the KDE6 install-directory and compiler settings modules.
- Detect the installed KDecoration package and define one internal compatibility selection used by targets and source adapters.

- [ ] **Step 3: Port the common library target**

In [`libbreezecommon/CMakeLists.txt`](../libbreezecommon/CMakeLists.txt:4):

- Use `find_package(Qt6 REQUIRED CONFIG COMPONENTS Widgets)`.
- Rename `breeze10common5` to `breeze10common6` in `add_library`, export-header generation, link configuration, properties, and install rules.
- Replace `Qt5::Core` and `Qt5::Gui` with Qt6 targets.

- [ ] **Step 4: Separate decoration and KCM source lists**

In [`CMakeLists.txt`](../CMakeLists.txt:63):

- Keep decoration runtime sources in the decoration target.
- Remove configuration widget sources from that target.
- Add a `config` subdirectory or equivalent target definition that owns the existing configuration widgets, UI forms, generated settings code, and the new KCM entry point.
- Ensure generated `breezesettings` code is compiled exactly once per target and does not create duplicate symbols through accidental shared compilation.

- [ ] **Step 5: Add the Plasma 6 KCM entry point**

Create `config/kcm_breeze10.cpp` with this structure:

```cpp
#include "breezeconfigwidget.h"
#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(Breeze::ConfigWidget, "kcm_breeze10.json")

#include "kcm_breeze10.moc"
```

Create `config/kcm_breeze10.json` containing the KCM plugin metadata required by the Plasma 6 loader, including the Breeze10 name, description, plugin identifier, and KCM service type used by the selected KF6 baseline.

- [ ] **Step 6: Define the separate KCM target**

Use `kcoreaddons_add_plugin` for the KCM target. Link it against the Qt6 and KF6 components required by the existing widgets, including ConfigCore, CoreAddons, GuiAddons, I18n, IconThemes, and KCMUtils. Install it in the KDecoration KCM namespace supplied by the selected package.

Use `kcmutils_generate_desktop_file` to generate discoverable desktop metadata. Keep the generated metadata tied to the KCM target instead of retaining the legacy hand-written service files.

- [ ] **Step 7: Port decoration target links and install paths**

Update the decoration target to link `breeze10common6`, Qt6, KF6, and the selected KDecoration target. Install it using the KDecoration/KDE6 plugin-directory variable rather than a hard-coded `org.kde.kdecoration2` path. Preserve the decoration JSON metadata and validate its service type and KCM association against the Plasma 6.0 baseline.

- [ ] **Step 8: Configure the target split before source API edits**

Run:

```sh
cmake -S . -B build-kde6-stage1 -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
```

Expected: CMake reaches generation with the decoration and KCM targets visible. If generation fails, fix only dependency, target, install-namespace, or metadata wiring errors before proceeding to source API changes.

- [ ] **Step 9: Review the generated install graph**

Inspect the generated build files and install manifest for:

- One common library target named `breeze10common6`.
- One decoration module.
- One independent KCM module.
- Generated KCM metadata in the expected KDE6 service location.
- No installation of the obsolete hand-written KCM desktop files.

- [ ] **Step 10: Commit the build-system stage**

```sh
git add CMakeLists.txt libbreezecommon/CMakeLists.txt config/kcm_breeze10.cpp config/kcm_breeze10.json breeze.json

git commit -m "build: split Breeze10 KDE6 plugins"
```

Do not remove the legacy desktop files in this commit unless staged metadata inspection has already confirmed that the generated KCM is discoverable.

---

## Stage 2: Client API and Qt6 Source Port

**Files:**
- Modify: [`breezedecoration.cpp`](../breezedecoration.cpp:1)
- Modify: [`breezedecoration.h`](../breezedecoration.h:1)
- Modify: [`breezebutton.cpp`](../breezebutton.cpp:1)
- Modify: [`breezesettingsprovider.cpp`](../breezesettingsprovider.cpp:1)
- Modify: [`breezesizegrip.cpp`](../breezesizegrip.cpp:1)
- Modify: [`config/breezeexceptionlistwidget.cpp`](../config/breezeexceptionlistwidget.cpp:1)
- Create: [`breezecompat.h`](../breezecompat.h:1) for the localized client-access compatibility boundary

**Interfaces:**
- Consumes: the target and compile definitions from Stage 1.
- Produces: source that compiles against the Plasma 6.0 KDecoration API without changing runtime behavior.

- [ ] **Step 1: Add a focused client-access compatibility boundary**

Create [`breezecompat.h`](../breezecompat.h:1) and define one helper with an explicit return type matching the Plasma 6.0 KDecoration2 baseline. If the selected later KDecoration package uses a different namespace or accessor shape, implement that alternative in the same header under the compile definition from Stage 1:

```cpp
inline KDecoration2::DecoratedClient *decorationClient(KDecoration2::Decoration *decoration)
{
    return decoration ? decoration->client() : nullptr;
}
```

Keep later KDecoration compatibility in this header through the compile definition selected by Stage 1. Do not add repeated version checks to every call site.

- [ ] **Step 2: Replace decoration client smart-pointer access**

In [`breezedecoration.cpp`](../breezedecoration.cpp:194) and [`breezedecoration.h`](../breezedecoration.h:182), replace only client-specific `.data()` calls with the compatibility accessor or raw pointer form. Do not remove `.data()` from `settings()` or `QPointer`/`QSharedPointer` values that remain smart pointers.

Preserve existing null checks and signal connections to `DecoratedClient`.

- [ ] **Step 3: Replace button client access and typed icon connection**

In [`breezebutton.cpp`](../breezebutton.cpp:57), use the selected client pointer directly and replace the old icon connection with typed syntax:

```cpp
connect(decoration->client(), &KDecoration2::DecoratedClient::iconChanged,
        this, &Button::update);
```

Use the compatibility accessor if required by the selected KDecoration package. Keep existing button visibility and icon-update behavior unchanged.

- [ ] **Step 4: Replace settings-provider client access**

In [`breezesettingsprovider.cpp`](../breezesettingsprovider.cpp:78), adapt the client pointer before accessing `windowId()`. Keep `KWindowInfo` construction and exception-selection logic unchanged until compiler diagnostics identify a KF6 signature change.

- [ ] **Step 5: Replace size-grip client access**

In [`breezesizegrip.cpp`](../breezesizegrip.cpp:65), update constructor, embedding, position, and move/resize paths. Replace expressions such as `m_decoration.data()->client().data()` with a raw decoration pointer followed by the compatibility client accessor. Do not alter XCB behavior in this stage.

- [ ] **Step 6: Replace QRegExp with QRegularExpression**

In [`breezesettingsprovider.cpp`](../breezesettingsprovider.cpp:130), construct `QRegularExpression` from the exception pattern and use its match/index result while preserving substring matching behavior.

In [`config/breezeexceptionlistwidget.cpp`](../config/breezeexceptionlistwidget.cpp:326), replace `QRegExp::isValid()` with `QRegularExpression::isValid()` and preserve the existing validation loop and user-facing error behavior.

Add the required QtCore include explicitly where the source does not already receive it transitively.

- [ ] **Step 7: Compile the source batch**

Run:

```sh
cmake --build build-kde6-stage1 --parallel
```

Expected: no `client().data()` type errors, no missing `QRegExp` symbols, and no duplicate KCM/decorator symbols. Fix only errors caused by this stage before starting X11 changes.

- [ ] **Step 8: Review the diff for ownership mistakes**

Search the modified source:

```sh
rg "client\(\)\.data|QRegExp|settings\(\)\.data|m_decoration\.data" breezedecoration.cpp breezedecoration.h breezebutton.cpp breezesettingsprovider.cpp breezesizegrip.cpp config
```

Expected: no client accessor remains incorrectly treated as a Qt smart pointer; valid settings and decoration smart-pointer uses remain where their types require them.

- [ ] **Step 9: Commit the source API stage**

```sh
git add breezedecoration.cpp breezedecoration.h breezebutton.cpp breezesettingsprovider.cpp breezesizegrip.cpp config/breezeexceptionlistwidget.cpp

git commit -m "port: adapt Breeze10 sources to Qt6 APIs"
```

## Stage 3: Qt6 Native X11 Port

**Files:**
- Modify: [`breezedecoration.cpp`](../breezedecoration.cpp:48)
- Modify: [`breezesizegrip.cpp`](../breezesizegrip.cpp:29)
- Modify: [`config/breezedetectwidget.cpp`](../config/breezedetectwidget.cpp:39)
- Modify: [`config/breezeexceptiondialog.cpp`](../config/breezeexceptiondialog.cpp:30)
- Create: [`breezex11.h`](../breezex11.h:1) for the X11 helper contract
- Create: [`breezex11.cpp`](../breezex11.cpp:1) for root-window derivation and native-interface access

**Interfaces:**
- Consumes: `BREEZE_HAVE_X11`, XCB linkage, and the client-access result from Stage 2.
- Produces: checked Qt6 native X11 access with the same XCB operations and safe Wayland behavior.

- [ ] **Step 1: Define the native-interface access contract**

Use Qt6’s `QNativeInterface::QX11Application` from the application object. Every X11 operation must follow this order:

```cpp
#if BREEZE_HAVE_X11
const auto *x11 = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
if (!x11) {
    return;
}
xcb_connection_t *connection = x11->connection();
#endif
```

Use the exact `QGuiApplication`/native-interface includes required by the installed Qt6 headers. Never dereference the native interface before the null check.

- [ ] **Step 2: Add a root-window helper**

Replace each `QX11Info::appRootWindow()` call with a helper that derives the root window from the XCB setup and the default screen. The helper must return `XCB_WINDOW_NONE` when the connection or screen data is unavailable.

Use an explicit contract such as:

```cpp
xcb_window_t breezeX11RootWindow(xcb_connection_t *connection);
```

Keep this helper limited to X11 platform plumbing. It must not own or disconnect the Qt-managed XCB connection.

- [ ] **Step 3: Port the decoration X11 path**

In [`breezedecoration.cpp`](../breezedecoration.cpp:737), remove `QX11Info` and gate the existing XCB property path through the native-interface check. Preserve the current behavior for setting title-bar opacity/native window properties. On Wayland, return without attempting XCB access.

- [ ] **Step 4: Port the size-grip X11 path**

In [`breezesizegrip.cpp`](../breezesizegrip.cpp:82), [`breezesizegrip.cpp`](../breezesizegrip.cpp:99), [`breezesizegrip.cpp`](../breezesizegrip.cpp:191), and [`breezesizegrip.cpp`](../breezesizegrip.cpp:209):

- Replace `QX11Info::isPlatformX11()` with native-interface presence checks.
- Replace `QX11Info::connection()` with the checked native-interface connection.
- Replace `QX11Info::appRootWindow()` with the root helper.
- Preserve reparenting, stacking, geometry, synthetic release, `_NET_WM_MOVERESIZE`, and flush operations.
- Return safely if the client, connection, root window, or required XCB reply is unavailable.

- [ ] **Step 5: Port window detection**

In [`config/breezedetectwidget.cpp`](../config/breezedetectwidget.cpp:39), replace QX11Info access in atom creation and selection logic. Keep the existing XCB atom and event behavior. Hide or disable the detection control when no native X11 interface is available.

- [ ] **Step 6: Port exception-dialog gating**

In [`config/breezeexceptiondialog.cpp`](../config/breezeexceptiondialog.cpp:30), replace the platform check with the same native-interface presence rule. Preserve the current UI behavior that hides the detect button outside X11.

- [ ] **Step 7: Search for stale QX11Info usage**

Run:

```sh
rg "QX11Info|X11Extras|appRootWindow" .
```

Expected: no source or CMake reference remains. Matches in [`plans/2026-09-17-kde6-migration-implementation-plan.md`](2026-09-17-kde6-migration-implementation-plan.md:1) are allowed because the plan records the migration, while active source, CMake, and user documentation must be clean.

- [ ] **Step 8: Build with XCB enabled**

Run:

```sh
cmake --build build-kde6-stage1 --parallel
```

Expected: all X11 code compiles against Qt6 native interfaces and XCB without invalid pointer conversions or missing headers.

- [ ] **Step 9: Configure without XCB when available**

Run a separate build with XCB discovery disabled or unavailable:

```sh
cmake -S . -B build-kde6-no-xcb -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DCMAKE_DISABLE_FIND_PACKAGE_XCB=ON
cmake --build build-kde6-no-xcb --parallel
```

Expected: configuration and compilation succeed, with X11-only code excluded.

- [ ] **Step 10: Commit the X11 stage**

```sh
git add breezedecoration.cpp breezesizegrip.cpp config/breezedetectwidget.cpp config/breezeexceptiondialog.cpp

git commit -m "port: replace QX11Info with Qt6 native X11"
```

## Stage 4: Metadata, Documentation, and Verification

**Files:**
- Modify: [`breeze.json`](../breeze.json:1) only if the selected Plasma 6 KDecoration metadata validation identifies a required field or service-type correction; otherwise leave it unchanged
- Modify: [`README.md`](../README.md:16)
- Modify: [`NEWS`](../NEWS:1) and/or [`ChangeLog`](../ChangeLog:1)
- Remove after validation: [`config/breeze10config.desktop`](../config/breeze10config.desktop:1)
- Remove after validation: [`config/breezedecorationconfig.desktop`](../config/breezedecorationconfig.desktop:1)

**Interfaces:**
- Consumes: completed build, decoration target, KCM target, Qt6 native X11 paths, and settings behavior from Stages 1–3.
- Produces: a staged-installable Plasma 6 plugin set with accurate documentation and recorded verification results.

- [ ] **Step 1: Validate KCM metadata generation**

Run a clean configure and inspect the generated metadata:

```sh
cmake -S . -B build-kde6-final -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build-kde6-final --parallel
cmake --install build-kde6-final --prefix "$PWD/stage-kde6"
```

Expected: the staging prefix contains the decoration module, the `breeze.json` metadata, the `breeze10common6` library, the KCM module, generated KCM desktop metadata, and the settings schema in KDE6 locations.

- [ ] **Step 2: Confirm no obsolete KCM registration remains**

Inspect the install manifest and source tree:

```sh
rg "breeze10config\.desktop|breezedecorationconfig\.desktop|kcmshell5|X-KDE-Library" .
```

Expected: no obsolete files are installed or referenced as the active KCM registration mechanism. Only then remove the two legacy service files and their CMake install references.

- [ ] **Step 3: Check plugin metadata and namespaces**

Use the installed KDE6 tools or metadata inspection utilities to verify:

- The decoration plugin is discoverable under the selected KDecoration namespace.
- The decoration’s JSON identifies Breeze10 and its KCM association.
- The KCM plugin has a valid KDE6 plugin identifier and KCModule service metadata.
- The generated KCM desktop file points to the new KCM module, not the old combined decoration module.

- [ ] **Step 4: Update documentation**

In [`README.md`](../README.md:16):

- Replace Qt5/KF5 package examples with generic Qt6/KF6/Plasma 6 dependencies.
- State that the project targets Plasma 6/KF6 and is KDE5-incompatible.
- Document the current CMake configure/build/install commands.
- Explain that the KCM is installed as a separate plugin.
- State that XCB is optional and X11-only detection/size-grip behavior is unavailable on Wayland.

In [`NEWS`](../NEWS:1) or [`ChangeLog`](../ChangeLog:1), record the KDE6-only compatibility break and the separate KCM integration.

- [ ] **Step 5: Run the complete build verification**

Run:

```sh
cmake --build build-kde6-final --parallel
ctest --test-dir build-kde6-final --output-on-failure
```

Expected: the full build succeeds and all configured tests pass. If no tests are registered, record that result and rely on staged-install and runtime checks below.

- [ ] **Step 6: Test the X11 runtime path**

In an X11 Plasma 6 session, install the staged build and verify:

- Breeze10 appears in Window Decorations.
- The separate KCM opens.
- Settings save and reload, including opacity, exceptions, font, borders, buttons, shadows, animations, and maximized behavior.
- Window detection identifies the selected window.
- The size grip embeds, stacks, moves, and sends resize requests as before.
- KWin reloads without plugin or unresolved-symbol errors.

- [ ] **Step 7: Test the Wayland runtime path**

In a Wayland Plasma 6 session, verify:

- Breeze10 loads and renders.
- The window-detection control is hidden or disabled.
- Size-grip XCB operations are skipped.
- No crash or invalid native-interface dereference occurs during decoration creation, configuration loading, or KWin reload.

- [ ] **Step 8: Review the final diff and stale dependency scan**

Run:

```sh
rg "Qt5|KF5|QX11Info|X11Extras|QRegExp|breeze10common5|SERVICES_INSTALL_DIR|PLUGIN_INSTALL_DIR|kcmshell5" .
git diff --check
git status --short
```

Expected: no stale migration-era dependencies remain in active source/build/documentation paths, whitespace validation passes, and only intended files are changed.

- [ ] **Step 9: Commit metadata, documentation, and cleanup**

```sh
git add CMakeLists.txt libbreezecommon/CMakeLists.txt breeze.json README.md NEWS ChangeLog config

git commit -m "port: finish Breeze10 Plasma 6 integration"
```

## Plan Self-Review

- [ ] Every design requirement maps to Stage 1, 2, 3, or 4.
- [ ] No task requires a file or function that is not named in its task interface.
- [ ] KDecoration2 remains the Plasma 6.0 baseline; later compatibility stays localized.
- [ ] The separate KCM is implemented with a concrete entry-point shape and generated metadata.
- [ ] X11 behavior remains optional and guarded on both compile-time and runtime boundaries.
- [ ] Settings schema and user-visible behavior remain unchanged.
- [ ] Verification covers configure, build, staged install, X11, Wayland, metadata, and stale-reference scans.

## Completion Criteria

The plan is complete when all stage checklists pass, the clean Qt6/KF6 build succeeds, independently discoverable decoration and KCM plugins are present in the staged install, X11 and Wayland acceptance checks pass, and the documentation accurately describes the KDE6-only result.
