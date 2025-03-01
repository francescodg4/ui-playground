#pragma once

#include <QColor>
#include <QFont>
#include <QString>

/// Colours and the application style sheet of the holographic PDA look.
namespace Theme {

inline const QColor backdropTop { 0x1a, 0x7f, 0xd0 };
inline const QColor backdropBottom { 0x06, 0x2a, 0x4d };
inline const QColor bezelTop { 0x3a, 0xa0, 0xff };
inline const QColor bezelBottom { 0x15, 0x5c, 0xcf };
inline const QColor screenTop { 40, 125, 200 };
inline const QColor screenBottom { 10, 62, 115 };

inline const QColor text { 0xee, 0xf8, 0xff };
inline const QColor textDim { 0xa9, 0xcf, 0xe8 };
inline const QColor line { 185, 230, 255, 140 };
inline const QColor lineSoft { 185, 230, 255, 56 };
inline const QColor cyan { 0x7f, 0xe6, 0xff };
inline const QColor gold { 0xf3, 0xc7, 0x42 };
inline const QColor green { 0x5e, 0xf0, 0xb4 };
inline const QColor active { 0x1f, 0x86, 0xff };
inline const QColor badge { 0xf6, 0xa6, 0x23 };

/// Global Qt style sheet (scroll bars, buttons, labels, item views).
QString styleSheet();

/// Uppercase, letter-spaced font used by section titles.
QFont titleFont();

} // namespace Theme
