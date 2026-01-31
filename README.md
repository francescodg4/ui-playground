# Widget gallery: interface designs in C++ / Qt 6

A small Qt Widgets application that shows the most used standard widgets in one window and draws
them in several interface designs. Pick a design and the whole window restyles at once.

## Widgets

- **Buttons**: push (default, toggle, flat, disabled), tool button with a menu, radio buttons, check boxes
  (including tri-state)
- **Input**: line edits (placeholder, password), spin box, date-time edit, combo boxes (plain and editable)
- **Ranges**: slider with ticks, progress bar, scroll bar, a vertical-slider equalizer
- **Views**: a tab widget with a table, a tree, an icon list and rich text
- **Telemetry**: LCD numbers and a dial
- the menu bar, menus, combo box popups, tooltips and the status bar

**Design** (or the Interface menu, Ctrl+1 … 9) selects the interface and remembers the choice;
**Dark mode** (Ctrl+D) switches the designs that have one between light and dark; **Disable widgets** shows
the disabled states.

## Designs

Each design draws the widgets through its own `QStyle`, following its own design rules.

### Liquid Glass (`glass`)

Frosted cards over an ambient canvas that they blur and refract, recessed glass fields, clay buttons and
knobs that compress when pressed, and glows instead of outlines for active states.

### Emerald (`emerald`)

A GBA-era storage-box system (Pokémon Emerald), drawn on a pixel grid scaled up 2× with no smoothing:
cards under pill-shaped box banners on a tiled wallpaper, capsule keys that sink into their hard shadow,
lined-paper fields, an HP-bar progress, a CRT display and highlighted tiles for selection.

### Winamp (`winamp`)

The Winamp Modern skin: brushed metal, capsule buttons, fields and views behind cobalt LCD glass with
glowing text, LED indicators, VU-meter progress, EQ-style slider thumbs and the skin's slanted tabs.

### Metro (`metro`)

The Windows 8 Modern UI: flat and square on a deep `#1d1d1d` canvas, no shadows. Content is chrome, so a
group is only its large light title; buttons are 2px outlines that invert when pressed; fields, lists and
menus are white surfaces; LCD displays are accent live tiles and tabs are text headers with an accent bar.

### Revolut (`revolut`)

The Revolut.com interface model in the Idetica identity: a light canvas with large rounded cards that each
own one accent (violet, indigo, amber, magenta) under an uppercase eyebrow, violet and chip pill buttons,
white widget surfaces with an amber focus ring, pill tabs, and LCD displays on the dark violet-glow stage.

### Flat (`flat`)

A dark operations console (video-analytics / monitoring dashboard): navy panels with 1px hairlines on a
black canvas, no gradients or shadows; muted labels and light values; sunken fields and views; docked text
tabs with a steel-blue indicator; detection green only for check marks, progress and the primary action.

### Claude Code (`claudecode`)

A Claude x VS Code hybrid: square dark chrome in warm neutrals (panels titled like sidebar sections,
sunken fields, compact trees and tables, flat tabs whose active one carries a coral top border), rich text on
rounded cream paper, and one coral accent for the primary button, marks, fills, focus and LCD digits.

### Visual Studio Code (`vscode`)

The Dark+ look: flat grey chrome separated by 1px borders, panels titled like sidebar sections, `#3c3c3c`
inputs, 22px list rows with the blue selection, tabs rounded at the top, lightly rounded primary / secondary buttons, the
2px progress bar, and a single blue accent with an always-visible 1px focus ring.

### Proxmox (`proxmox`)

The Proxmox VE web console (ExtJS Crisp): square white panels with a pale header strip and a blue title on a
light-grey page, white toolbar buttons and fields with 2px radii, solid-blue primary actions and active tabs,
the resource tree's outlined selection, thin usage bars with the label above, and scroll bars with step
buttons. It has a dark mode that keeps the structure and the blue accent on dark-grey surfaces.

## Build & run

```bash
cmake -S . -B build
cmake --build build -j
./build/gallery                     # opens with the last design selected
./build/gallery --theme emerald     # or pick one
```

Requires Qt ≥ 6.4 (Widgets). Nothing else is downloaded.

| Option | |
|---|---|
| `--theme <id>` | design to use (see above); the default is the last one selected |
| `--dark` | use the dark mode of the designs that have one |
| `--screenshot <dir>` | save the window as `gallery-<theme>.png` and quit; `--theme all` saves every design; works with `-platform offscreen` |

## Code

```
src/
  main.cpp              command line, screenshot mode
  WidgetGallery         the window: the widgets and the design selector
  WidgetStyle           base QStyle of the designs: the plumbing (states, geometry, which primitive a
                        widget asks for) and a small drawing vocabulary (button, field, slider, tab,
                        dial...) each design fills in
  ThemeRegistry         the list of designs and the remembered choice
  Icons                 vector icons of the item views
  themes/<id>/          one folder (and C++ namespace) per design:
    Entry.cpp           registers the design (name, description, style factory)
    Style               the design's WidgetStyle
    Theme               its design tokens
    widgets/            its drawing helpers (Glass, Pixel, Metal)
```

Adding a design means adding a folder under `themes/` with an `Entry.cpp` and a `Style`, and one line in
`ThemeRegistry.cpp`.
