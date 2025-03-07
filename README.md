# PDA: interface demo in C++ / Qt 6 (Emerald design)

A small Qt Widgets application that recreates the PDA screens,
one page per entry of the top menu plus a gallery of the images acquired by the device.

This branch styles it as a GBA-era storage-box system (Pokémon Emerald):

- **Pixel grid**: everything is drawn on a low-resolution grid without antialiasing and scaled up 2×
  with nearest-neighbour sampling; sprites are outlined and reduced to 15-bit colour; text uses a
  monospace pixel font with bi-colour drop shadows.
- **Box wallpapers**: every page sits in a wallpaper container (tiled 16×16 pattern, bi-colour frame) under
  a pill-cornered header banner — Forest, Metallic, Sky, Seafloor, Simple and River.
- **Top menu**: a row of item tiles with the pointing-glove cursor over the current one, above the box-name
  banner with bi-colour arrows (click them, or use Q / E like the L / R buttons).
- **Components**: metallic data windows with a blue CRT screen and scanlines (photo preview, viewer, scans),
  a teal party deck (equipped items, encyclopedia topics), cream lined-paper text boxes with a double border,
  beveled capsule keys that drop into their hard pixel shadow when pressed, highlighted tiles for selection.
- Pages switch instantly: the design has no transitions or animation.

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
| `--screenshot <dir>` | save every page (and the photo viewer) as PNG and quit; works with `-platform offscreen` |

## Pages

| Tab | Page | Interaction |
|---|---|---|
| 👤 | **Inventory** | storage box and the equipped items as a party deck; click an item to see its name |
| 🔧 | **Blueprints** | craftable items by category |
| 📍 | **Ping Manager** | show/hide each ping (or all of them) and pick its colour |
| 🖼 | **Photo Manager** | box of the acquired images with a data-window preview; *OK VIEW* or a double click opens the viewer (← / →, Del deletes, Esc / *B BACK* returns); *IMPORT* copies images into the folder |
| 📄 | **Log** | messages grouped by day, with a PLAY key |
| 📖 | **Encyclopedia** | collapsible topic tree; the open entry shows its scan on a data window and its text |

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
  PdaWindow             emerald backdrop, box header + the current page in its wallpaper container
  Theme                 indexed palette and the Qt style sheet
  Icons                 every icon drawn with QPainter from SVG-like path data (no image assets)
  PhotoLibrary          the acquired images: folder scan, watch, import, delete
  widgets/Pixel         pixel canvas, wallpapers, banners, data windows, text boxes, keys, glove, sprites
  widgets/PixelWidgets  pixel label, capsule key, paper box, deck panel, banner strip, sprite label
  widgets/BoxHeader     top menu: item tiles, glove cursor, box-name banner and arrows
  pages/                one class per tab
```
