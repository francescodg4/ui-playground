# PDA: interface demo in C++ / Qt 6

A small Qt Widgets application that recreates the PDA screens:
one page per entry of the top menu, plus a gallery of the images acquired by the device.

The interface comes in several themes. Each one is a complete design (its own layout, controls and
drawing code), and the user can switch between them while the app runs.

## Themes

Every theme has a **Select theme** control. Ctrl+T works too. It opens a dialog with a preview of each
theme. The whole window switches at once, keeping the current page, size and position, and the choice
is remembered for the next start. `--theme <id>` overrides it.

### Liquid Glass (`glass`)

- Frosted glass over a slowly drifting ambient canvas. The tab bar and the photo viewer blur and refract
  what lies beneath them (12 / 18 / 24 px by elevation), and the tint adapts to the backdrop's brightness.
- Bento cards, clay controls that compress when pressed, and glows instead of outlines.
- Pages change with a short slide and cross-fade.
- **Select theme**: the palette button at the right of the tab bar.

### Emerald (`emerald`)

- A GBA-era storage-box system (Pokémon Emerald). Everything is drawn on a pixel grid scaled up 2× with
  no smoothing. Sprites are outlined and text has two-colour drop shadows.
- Each page sits on its own tiled box wallpaper under a pill-shaped banner.
- The top menu is a row of item tiles with a pointing-glove cursor; the arrows beside the banner (or
  Q / E) step through the pages.
- Other components: data windows with a CRT screen and scanlines, a teal party deck, lined-paper text
  boxes and capsule keys that sink into their shadow.
- Pages switch instantly; the design has no animation.
- **Select theme**: the `SELECT` key at the right of the banner.

### Winamp (`winamp`)

- The Winamp Modern skin: brushed-metal frame, custom title bar (drag to move; minimise / maximise /
  close), and a textual menu that selects the page (Alt + underlined letter).
- A cobalt LCD shows telemetry: a 7-segment clock, counters, a status badge, a spectrum visualizer and a
  dot-matrix status line (static: a line too long for it ends with an ellipsis).
- Capsule and round transport buttons. The photo viewer is driven like a player: slider, previous /
  slideshow / pause / stop / next.
- Pages switch instantly; only the visualizer moves.
- **Select theme**: the capsule at the right end of the menu row.

## Build & run

```bash
cmake -S . -B build
cmake --build build -j
./build/pda                         # opens with the last theme selected
./build/pda --theme emerald         # or pick one
```

Requires Qt ≥ 6.4 (Widgets). Nothing else is downloaded.

| Option | |
|---|---|
| `--theme <id>` | theme to use (see above); the default is the last one selected |
| `--photos <dir>` | folder of acquired images (default: [photos/](photos/) in the source tree) |
| `--page <n>` | page shown at start, 1–6 |
| `--fullscreen` | run full screen, e.g. on the device display (F11 toggles) |
| `--reduced-motion` | no animations (transitions, moving canvas, visualizer, pulsing) |
| `--reduced-transparency` | solid surfaces instead of glass (Liquid Glass) |
| `--screenshot <dir>` | save every page (and the photo viewer) as PNG and quit; `--theme all` writes one folder per theme; works with `-platform offscreen` |

## Pages

| Page | What it shows |
|---|---|
| **Inventory** | storage grid and equipped items; select an item to see its name |
| **Blueprints** | craftable items by category |
| **Ping Manager** | show / hide each ping (or all of them) and pick its colour |
| **Photo Manager** | the acquired images and a viewer (← / →, Del deletes, Esc returns); *Import* copies images into the folder |
| **Log** | messages grouped by day, each with a play button |
| **Encyclopedia** | collapsible topic tree; the open entry shows its bio scan and text |

Keys: `1`–`6` jump to a page, `Q` / `E` go to the previous / next one, Ctrl+T selects the theme.

## Photo gallery

The Photo Manager shows the `png`, `jpg`, `jpeg`, `bmp` and `gif` files of the photos folder, oldest first.
The folder is watched, so images saved there by the acquisition software appear without a restart.
The images in [photos/](photos/) are samples.

## Code

```
src/
  main.cpp              command line, screenshot mode
  Icons, PhotoLibrary   shared by all themes: vector icons, the folder of acquired images
  PdaShell              base class of every theme's main window
  ThemeRegistry         the list of themes and the remembered choice
  ThemeController       builds the window of a theme and swaps it when another one is selected
  ThemeDialog           the "Select theme" dialog
  themes/<id>/          one folder (and C++ namespace) per theme:
    Entry.cpp           registers the theme (name, style sheet, font, window factory)
    PdaWindow, Theme    the theme's main window and design tokens
    pages/, widgets/    the six pages and the theme's own widgets
```

Adding a theme means adding a folder under `themes/` with an `Entry.cpp` and one line in `ThemeRegistry.cpp`.
