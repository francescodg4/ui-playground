#pragma once

#include <QColor>
#include <QFont>

namespace metro {

/// Tokens of the Windows 8 Modern UI ("Metro"): flat saturated accents on a deep neutral canvas,
/// typography instead of boxes, no shadows and no rounding.
namespace Theme {

// canvas and surfaces
inline const QColor canvas { 0x1d, 0x1d, 0x1d }; ///< --metro-bg-dark
inline const QColor light { 0xf2, 0xf2, 0xf2 }; ///< --metro-bg-light
inline const QColor charms { 0x11, 0x11, 0x11 }; ///< the charms bar
inline const QColor white { 0xff, 0xff, 0xff }; ///< --metro-tile-fg

// accents
inline const QColor teal { 0x00, 0xb7, 0xc3 };
inline const QColor blue { 0x1b, 0xa1, 0xe2 };
inline const QColor purple { 0x7e, 0x38, 0xb7 };
inline const QColor magenta { 0xd8, 0x00, 0x73 };
inline const QColor red { 0xe5, 0x14, 0x00 };
inline const QColor orange { 0xe3, 0xa2, 0x1a };
inline const QColor green { 0x10, 0x7c, 0x41 };
inline const QColor accent = teal; ///< primary accent, default focus

// supporting greys
inline const QColor text { 0x1d, 0x1d, 0x1d }; ///< text on light surfaces
inline const QColor dim { 0x8a, 0x8a, 0x8a }; ///< borders, disabled text on the canvas
inline const QColor track { 0x46, 0x46, 0x46 }; ///< empty part of sliders and progress
inline const QColor hover { 0xde, 0xde, 0xde }; ///< hovered item on light surfaces

/// Segoe UI (with fallbacks) at @p pointSize and @p weight.
QFont font(qreal pointSize = 10.5, QFont::Weight weight = QFont::Normal);

} // namespace Theme

} // namespace metro
