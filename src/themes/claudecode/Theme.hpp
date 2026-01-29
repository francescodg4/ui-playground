#pragma once

#include <QColor>
#include <QFont>

namespace claudecode {

/// Tokens of the Claude x VS Code hybrid: dense dark chrome in warm neutrals, cream paper for
/// reading surfaces, and one coral accent.
namespace Theme {

// chrome
inline const QColor chrome0 { 0x18, 0x17, 0x16 }; ///< --chrome-0: canvas, fields
inline const QColor chrome1 { 0x1e, 0x1c, 0x1a }; ///< --chrome-1: panels, tabs
inline const QColor chromeHover { 0x2a, 0x28, 0x22 }; ///< --chrome-hover
inline const QColor chromeSelected { 0x33, 0x2f, 0x28 }; ///< --chrome-selected
inline const QColor chromeBorder { 0x2a, 0x28, 0x22 }; ///< --chrome-border
inline const QColor chromeBorderHover { 0x3a, 0x37, 0x2f };
inline const QColor chromeText { 0xcf, 0xca, 0xc0 }; ///< --chrome-text
inline const QColor chromeTextMuted { 0x8a, 0x85, 0x7c }; ///< --chrome-text-muted
inline const QColor chromeTextBright { 0xf2, 0xf0, 0xea }; ///< --chrome-text-bright
inline const QColor chromeTextDisabled { 0x5a, 0x56, 0x4d };

// editor and paper
inline const QColor editor { 0x1e, 0x1e, 0x1e }; ///< --editor-bg
inline const QColor paper { 0xf7, 0xf4, 0xec }; ///< --paper-bg
inline const QColor paperBorder { 0xe8, 0xe2, 0xd3 }; ///< --paper-border
inline const QColor paperText { 0x2b, 0x29, 0x24 }; ///< --paper-text
inline const QColor paperTextMuted { 0x6f, 0x6a, 0x5e }; ///< --paper-text-muted

// accent
inline const QColor accent { 0xd9, 0x77, 0x57 }; ///< --accent: the only saturated colour
inline const QColor accentHover { 0xc8, 0x6a, 0x4a }; ///< --accent-hover
inline const QColor accentTint { 217, 119, 87, 31 }; ///< --accent-tint (12%)
inline const QColor accentDisabled { 0xd9, 0xd3, 0xc3 };
inline const QColor accentDisabledText { 0xa3, 0x9c, 0x8c };

// radii
inline constexpr qreal radiusSmall = 4; ///< status chip, badges
inline constexpr qreal radiusButton = 8; ///< buttons, code blocks
inline constexpr qreal radiusCard = 12; ///< paper cards

/// --font-ui (Segoe UI / Inter) at @p pointSize and @p weight.
QFont font(qreal pointSize = 9.75, QFont::Weight weight = QFont::Normal);

} // namespace Theme

} // namespace claudecode
