#pragma once

#include <QColor>
#include <QFont>

namespace vscode {

/// Tokens of Visual Studio Code's Dark+ theme: neutral greys, flat surfaces separated by 1px
/// borders, 22px rows and a single blue accent.
namespace Theme {

// surfaces
inline const QColor editor { 0x1e, 0x1e, 0x1e }; ///< --editor-bg
inline const QColor sidebar { 0x25, 0x25, 0x26 }; ///< --sidebar-bg, --widget-bg
inline const QColor titlebar { 0x3c, 0x3c, 0x3c }; ///< --titlebar-bg
inline const QColor tabInactive { 0x2d, 0x2d, 0x2d }; ///< --tab-inactive-bg
inline const QColor input { 0x3c, 0x3c, 0x3c }; ///< --input-bg

// lines
inline const QColor border { 0x3c, 0x3c, 0x3c }; ///< --border
inline const QColor widgetBorder { 0x45, 0x45, 0x45 }; ///< --widget-border
inline const QColor checkBorder { 0x6b, 0x6b, 0x6b };

// text
inline const QColor fg { 0xcc, 0xcc, 0xcc }; ///< --fg
inline const QColor fgBright { 0xff, 0xff, 0xff }; ///< --fg-bright
inline const QColor fgMuted { 0x85, 0x85, 0x85 }; ///< --fg-muted
inline const QColor fgTab { 0x96, 0x96, 0x96 }; ///< inactive tab label
inline const QColor fgDisabled { 0x6b, 0x6b, 0x6b };
inline const QColor link { 0x37, 0x94, 0xff }; ///< --link

// interaction
inline const QColor hover { 0x2a, 0x2d, 0x2e }; ///< --hover
inline const QColor selected { 0x09, 0x47, 0x71 }; ///< --selected
inline const QColor selection { 0x26, 0x4f, 0x78 }; ///< --selection (editor text)
inline const QColor focus { 0x00, 0x7f, 0xd4 }; ///< --focus

// accent
inline const QColor accent { 0x00, 0x7a, 0xcc }; ///< --accent: status bar, active indicators
inline const QColor button { 0x0e, 0x63, 0x9c }; ///< --button-bg
inline const QColor buttonHover { 0x11, 0x77, 0xbb }; ///< --button-hover
inline const QColor buttonSecondary { 0x3a, 0x3d, 0x41 }; ///< --button-secondary-bg
inline const QColor buttonSecondaryHover { 0x45, 0x49, 0x4e };
inline const QColor progress { 0x0e, 0x70, 0xc0 };

// radii
inline constexpr qreal radiusSmall = 2; ///< buttons, chips
inline constexpr qreal radiusCheck = 3; ///< check boxes
inline constexpr qreal radiusTab = 6; ///< top corners of the tabs

/// --font-ui (Segoe UI / system-ui) at @p pointSize and @p weight.
QFont font(qreal pointSize = 9.75, QFont::Weight weight = QFont::Normal);

} // namespace Theme

} // namespace vscode
