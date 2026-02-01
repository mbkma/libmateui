# libmateui

**A GTK3-based helper library for MATE apps**

## Overview

`libmateui` is a **shared UI helper library for MATE desktop applications**.  

Its goal is to centralize common MATE application patterns, reduce boilerplate, and ensure consistent user experience across apps.

It is **not** a theme, a widget toolkit, or a GTK fork. Instead, it provides:

- Standardized **application and window helpers**
- **Menu management** (menubars, accelerators, GMenuModel support)
- **Dialogs** (About, Preferences)
- **GSettings bindings**
- **Session inhibit helpers**
- Miscellaneous **utilities** for common MATE patterns

By using `libmateui`, MATE apps no longer need to implement these behaviors independently, improving **consistency, stability, and maintainability**.

## Why `libmateui`?

MATE faces unique constraints:

| Constraint | Impact |
|------------|--------|
| Long-lived apps | Rewriting apps every GTK change is impractical |
| Traditional UX | Users expect menubars, tray icons, and global shortcuts |
| GTK3 commitment | GTK4 breaks many GTK3 assumptions |
| Small core team | Duplicated boilerplate is costly |

`libmateui` solves these problems by **centralizing solutions** to all of them:

- One place for menus, dialogs, accelerators, and session management
- GTK3 stability buffer against upstream GTK changes
- Enables gradual modernization (HiDPI, accessibility, consistent shortcuts)
- Explicitly encodes MATE’s UX philosophy
- Simplifies onboarding for contributors
- Provides potential cross-desktop collaboration (XFCE, GNOME-style helpers)


## Key Benefits

1. **Consistent UX across all MATE apps**
2. **Reduced per-app maintenance**
3. **Centralized GTK3 → GTK4 transition path**
4. **Encodes MATE’s traditional desktop philosophy**
5. **Improves contributor onboarding**

## GTK4 Transition Strategy

GTK3 cannot last forever — eventually MATE apps will need GTK4.  
`libmateui` prepares MATE for this transition with minimal disruption.

### Why a direct GTK3 → GTK4 jump is hard

| Feature | GTK3 | GTK4 |
|---------|------|------|
| Menubars | Native | Discouraged / awkward |
| Status icons | Supported | Removed |
| Accelerator paths | Supported | Removed |
| Widget subclassing | Easy | Restricted |
| X11 assumptions | Common | Wayland-first |

A naïve port risks breaking apps, UX, and maintainers’ sanity.

### libmateui as a GTK abstraction layer

Current flow:

```

[MATE apps] → GTK3

```

With libmateui:

```

[MATE apps] → libmateui → GTK3

```

Later, GTK4 can replace GTK3 inside libmateui:

```

[MATE apps] → libmateui → GTK4

````

Apps themselves **do not need to change**.  
This allows incremental, low-risk migration.

### How libmateui enables GTK4 migration

1. **Policy vs backend**: libmateui defines “how MATE behaves,” not “how GTK behaves.”
2. **Parallel GTK3/GTK4 backends**:  
   ```c
   #if MATEUI_USE_GTK4
     /* GTK4 implementation */
   #else
     /* GTK3 implementation */
   #endif
    ```

3. **Incremental app porting**: One app at a time can adopt GTK4 through libmateui.
4. **Emulation of missing GTK4 features**: Menubars, status icons, accelerators, global menus, etc., can be handled inside libmateui.


## Getting Started

See the [examples](examples/) folder for usage demos of:

* GtkApplication creation
* Windows and menubars
* About dialogs
* Accelerators
* Session management

## License

MIT

