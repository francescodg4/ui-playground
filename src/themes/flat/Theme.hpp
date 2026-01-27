#pragma once

#include <QColor>
#include <QFont>

namespace flat {

/// Tokens of the flat dark operations console: navy hairline panels on a black canvas, muted labels
/// and light values, one signal colour (detection green) and monospace for machine data.
namespace Theme {

// canvas and surfaces
inline const QColor canvas { 0x05, 0x08, 0x0d }; ///< --flat-canvas
inline const QColor panel { 0x0a, 0x11, 0x1b }; ///< --flat-panel
inline const QColor sunken { 0x08, 0x0b, 0x12 }; ///< --flat-panel-sunken: console, fields
inline const QColor border { 0x16, 0x20, 0x2c }; ///< --flat-panel-border
inline const QColor divider { 0x12, 0x1c, 0x27 }; ///< --flat-divider
inline const QColor rowHover { 0x0f, 0x1a, 0x26 }; ///< --flat-row-hover
inline const QColor selected { 0x14, 0x22, 0x31 }; ///< --flat-selected
inline const QColor selectedLine { 0x5e, 0x7c, 0x96 }; ///< --flat-selected-line: indicators, focus
inline const QColor button { 0x10, 0x1a, 0x26 };
inline const QColor buttonPressed { 0x1a, 0x2c, 0x40 };
inline const QColor borderHover { 0x24, 0x32, 0x4a };
inline const QColor borderDisabled { 0x10, 0x18, 0x21 };

// text
inline const QColor textTitle { 0xc8, 0xd0, 0xda };
inline const QColor text { 0xc0, 0xc4, 0xca };
inline const QColor textMuted { 0x88, 0x8f, 0x99 };
inline const QColor textTab { 0x82, 0x89, 0x91 };
inline const QColor textSelected { 0xa8, 0xbc, 0xcf };
inline const QColor textDisabled { 0x4a, 0x52, 0x60 };
inline const QColor textOverlay { 0xe9, 0xe5, 0xe4 };

// signal and status
inline const QColor detect { 0x5c, 0xd6, 0x6b }; ///< tracked objects, check marks
inline const QColor detectFill { 0x3f, 0x8f, 0x55 }; ///< badges, primary action
inline const QColor detectHover { 0x4a, 0x9d, 0x61 };
inline const QColor detectPressed { 0x35, 0x7a, 0x48 };
inline const QColor detectText { 0xe3, 0xfd, 0xee };
inline const QColor detectDisabled { 0x1c, 0x2a, 0x22 };

inline constexpr qreal radius = 2; ///< --flat-radius

/// UI face (Segoe UI / Noto Sans) or the monospace face for machine data.
QFont font(qreal pointSize = 9.75, QFont::Weight weight = QFont::Normal);
QFont mono(qreal pointSize = 9);

} // namespace Theme

} // namespace flat
