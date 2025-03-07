# PDA: interface demo in C++ / Qt 6 (Winamp design)

A small Qt Widgets application that recreates the PDA screens,
one page per entry of the top menu plus a gallery of the images acquired by the device.

This branch skins it like Winamp Modern (skeuomorphic metal meets Frutiger Aero glass):

- **Title bar**: navy-to-black metallic gradient with reflection ridges, centred branding, working
  minimise / maximise / close controls (the window draws its own chrome; drag the bar to move it).
- **Menu**: inline textual entries under the title bar select the page (Alt + underlined letter).
- **Telemetry LCD**: cobalt glass with scanlines, a 7-segment clock (elapsed time while a log entry plays),
  counter badges, a PLAYING / ONLINE badge, a spectrum visualizer and a dot-matrix marquee reporting what
  is going on.
- **Controls**: brushed-metal frame with 1px bevels, recessed LCD panels with an inset shadow, capsule
  buttons, round bevelled transport buttons (blue accent on the active one), a position slider with a
  capsule thumb, indicator LEDs and bottom docking tabs.
- Pages switch instantly: the design defines no transitions. The marquee and visualizer move
  (`--reduced-motion` freezes them).

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
| `--reduced-motion` | freeze the marquee and the visualizer |
| `--screenshot <dir>` | save every page (and the photo viewer) as PNG and quit; works with `-platform offscreen` |

## Pages

| Menu | Page | Interaction |
|---|---|---|
| Inventory | **Inventory** | storage grid on the LCD, equipment as round metal buttons; selections show on the marquee |
| Blueprints | **Blueprints** | craftable items as a numbered playlist, grouped by category |
| Pings | **Ping Manager** | ON / OFF capsule per ping (or all of them), colour picked on indicator LEDs |
| Photos | **Photo Manager** | GALLERY / VIEWER docking tabs; the viewer has a position slider and a transport deck (previous, slideshow play / pause / stop, next — also ← / →, Space, Del, Esc); ▲ imports images, PL returns to the gallery |
| Log | **Log** | messages as a playlist; playing one runs the clock, the visualizer and the marquee |
| Encyclopedia | **Encyclopedia** | collapsible topic tree; the open entry shows its bio scan and text on the LCD |

Keys: Alt + underlined letter or `1`–`6` open a page, `Q` / `E` go to the previous / next one.

## Photo gallery

The Photo Manager shows the `png`, `jpg`, `jpeg`, `bmp` and `gif` files of the photos folder, oldest first.
The folder is watched, so images saved there by the acquisition software appear without a restart.
The images in [photos/](photos/) are samples.

## Code

```
src/
  main.cpp              command line, style, window
  PdaWindow             skinned window: title bar, menu, telemetry LCD and the page in the metal frame
  Theme                 skin tokens and the Qt style sheet
  Icons                 every icon drawn with QPainter from SVG-like path data (no image assets)
  PhotoLibrary          the acquired images: folder scan, watch, import, delete
  widgets/Metal         metal frame, bevels, LCD glass, capsules, round buttons, 7-segment, dot matrix, LEDs
  widgets/MetalWidgets  capsule / round buttons, LCD panel, position slider
  widgets/Chrome        title bar and menu strip
  widgets/Telemetry     the LCD: clock, counters, badge, visualizer, marquee
  pages/                one class per tab
```
