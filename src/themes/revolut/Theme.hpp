#pragma once

#include <QColor>
#include <QFont>

namespace revolut {

/// Tokens of the Revolut.com interface model in the Idetica identity: a light canvas, violet
/// brand accent, pastel-tinted cards that each own one accent, pill buttons, an amber focus ring.
namespace Theme {

// neutrals (light)
inline const QColor canvas { 0xfd, 0xfb, 0xff };
inline const QColor surface = QColor::fromHslF(266 / 360.0f, 0.60f, 0.97f); ///< cards
inline const QColor widget { 0xff, 0xff, 0xff }; ///< app widgets floating on the cards
inline const QColor text { 0x1a, 0x0f, 0x33 }; ///< primary
inline const QColor textSecondary { 0x4a, 0x41, 0x66 };
inline const QColor textTertiary { 0x6f, 0x66, 0x8a };
inline const QColor chip = QColor::fromHslF(266 / 360.0f, 0.35f, 0.93f); ///< secondary buttons
inline const QColor hairline = QColor::fromHslF(266 / 360.0f, 0.30f, 0.90f);

// brand
inline const QColor accent { 154, 81, 248 }; ///< violet: primary buttons, active states
inline const QColor accentHover = QColor::fromHslF(266 / 360.0f, 0.80f, 0.50f);
inline const QColor focus { 0xf5, 0x9e, 0x0b }; ///< amber focus ring
inline const QColor success { 0x16, 0xa3, 0x4a };

// dark stage (hero and CTA bands)
inline const QColor stage { 0x07, 0x06, 0x0d };
inline const QColor stageText { 0xf1, 0xed, 0xf9 };

/// Card accent with its light tint: each card owns one pair.
struct Tint {
    QColor accent;
    QColor tint;
};
inline const Tint tints[] = {
    { accent, QColor::fromHslF(266 / 360.0f, 1.00f, 0.96f) }, // violet
    { QColor::fromHslF(250 / 360.0f, 0.92f, 0.56f), QColor::fromHslF(250 / 360.0f, 1.00f, 0.96f) }, // indigo
    { QColor(0xb4, 0x53, 0x09), QColor(0xff, 0xf4, 0xdc) }, // amber (its text shade on light)
    { QColor::fromHslF(289 / 360.0f, 0.75f, 0.52f), QColor::fromHslF(289 / 360.0f, 1.00f, 0.96f) }, // magenta
};

// radii (--rui-radius-*)
inline constexpr qreal radiusField = 12;
inline constexpr qreal radiusWidget = 16;
inline constexpr qreal radiusCard = 24;

/// Inter (with system fallbacks) at @p pointSize and @p weight.
QFont font(qreal pointSize = 10, QFont::Weight weight = QFont::Normal);

} // namespace Theme

} // namespace revolut
