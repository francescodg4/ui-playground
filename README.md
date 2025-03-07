# PDA: holographic interface demo in C++ / Qt 6

A small Qt Widgets application that recreates the PDA screens:
a translucent blue screen with a slanted tab bar at the top, one page per tab, and a gallery of the
images acquired by the device.

## Build & run

```bash
cmake -S . -B build
cmake --build build -j
./build/pda                         # opens on the Inventory page
```

Requires Qt ≥ 6.4 (Widgets). Nothing else is downloaded.

| Option | |
|---|---|
| `--photos <dir>` | folder of acquired images (default: [photos/](photos/) in the source tree) |
| `--page <n>` | page shown at start, 1–6 |
| `--fullscreen` | run full screen, e.g. on the device display (F11 toggles) |
| `--reduced-motion` | switch pages without the transition animation |
| `--screenshot <dir>` | save every page (and the photo viewer) as PNG and quit; works with `-platform offscreen` |

## Pages

| Tab | Page | Interaction |
|---|---|---|
| 👤 | **Inventory** | storage grid and equipment slots around the diver; click an item to see its name |
| 🔧 | **Blueprints** | craftable items by category |
| 📍 | **Ping Manager** | show/hide each ping (or all of them) and pick its colour |
| 🖼 | **Photo Manager** | gallery of the acquired images; click one to open the viewer (← / → or swipe, Del deletes, Esc closes); *Import* copies images into the folder |
| 📄 | **Log** | messages grouped by day, with a play button |
| 📖 | **Encyclopedia** | collapsible topic tree; the selected entry shows a bio-scan card and its text |

Keys: `1`–`6` jump to a page, `Q` / `E` go to the previous / next one.
The badges on the tabs clear when the tab is opened.

## Photo gallery

The Photo Manager shows the `png`, `jpg`, `jpeg`, `bmp` and `gif` files of the photos folder, oldest first.
The folder is watched, so images saved there by the acquisition software appear without a restart.
The images in [photos/](photos/) are samples.

## Code

```
src/
  main.cpp              command line, style, window
  PdaWindow             screen painting (rim, glow, light rays, dot grid, particles), tab bar + pages
  Theme                 colours and the Qt style sheet
  Icons                 every icon drawn with QPainter from SVG-like path data (no image assets)
  PhotoLibrary          the acquired images: folder scan, watch, import, delete
  widgets/TabBar        slanted tabs with notification badges
  widgets/Holo          section title (gold underline) and small shared helpers
  widgets/PageTransition  slide + cross-fade overlay played when the page changes
  pages/                one class per tab
```
