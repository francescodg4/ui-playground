#pragma once

#include <QColor>
#include <QFont>

namespace revolut {

/// Tokens of the Revolut.com interface model in the Idetica identity: a light canvas, violet
/// brand accent, pastel-tinted cards that each own one accent, pill buttons, an amber focus ring.
/// The dark mode follows the identity's dark column: a near-black canvas, deep violet-grey cards
/// and widgets, a brighter violet, dark tints and translucent-white secondary buttons.
namespace Theme {

/// Card accent with its tint: each card owns one pair.
struct Tint {
    QColor accent;
    QColor tint;
};

struct Colors {
    // neutrals
    QColor canvas;
    QColor surface; ///< cards
    QColor widget; ///< app widgets floating on the cards
    QColor text; ///< primary
    QColor textSecondary;
    QColor textTertiary;
    QColor hairline;
    QColor border; ///< check box and radio button outlines

    // secondary buttons: a violet-grey chip on light, translucent white on dark
    QColor chip;
    QColor chipHover;
    QColor chipPressed;

    // brand
    QColor accent; ///< violet: primary buttons, active states
    QColor accentHover;

    // controls
    QColor track; ///< empty part of sliders, progress and dial
    QColor selected; ///< selected item, active menu item
    QColor rowHover;
    QColor alternate; ///< alternate rows, hovered headers
    QColor disabledFill;
    QColor disabledText;
    QColor tooltip;
    QColor tooltipText;

    Tint tints[4]; ///< violet, indigo, amber, magenta
};

const Colors& light();
const Colors& dark();

// the same in both modes
inline const QColor focus { 0xf5, 0x9e, 0x0b }; ///< amber focus ring
inline const QColor success { 0x16, 0xa3, 0x4a };
inline const QColor stage { 0x07, 0x06, 0x0d }; ///< dark stage (hero and CTA bands)
inline const QColor stageText { 0xf1, 0xed, 0xf9 };

// radii (--rui-radius-*)
inline constexpr qreal radiusField = 12;
inline constexpr qreal radiusWidget = 16;
inline constexpr qreal radiusCard = 24;

/// Inter (with system fallbacks) at @p pointSize and @p weight.
QFont font(qreal pointSize = 10, QFont::Weight weight = QFont::Normal);

} // namespace Theme

} // namespace revolut
