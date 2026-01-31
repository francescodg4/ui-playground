#include "themes/revolut/Theme.hpp"

#include <QStringList>

namespace revolut {

namespace Theme {

namespace {

    QColor hsl(int h, qreal s, qreal l, int alpha = 255)
    {
        QColor c = QColor::fromHslF(h / 360.0f, float(s), float(l));
        c.setAlpha(alpha);
        return c;
    }

} // namespace

const Colors& light()
{
    static const Colors c {
        // neutrals
        QColor(0xfd, 0xfb, 0xff), hsl(266, 0.60, 0.97), QColor(0xff, 0xff, 0xff),
        QColor(0x1a, 0x0f, 0x33), QColor(0x4a, 0x41, 0x66), QColor(0x6f, 0x66, 0x8a),
        hsl(266, 0.30, 0.90), QColor(0xc4, 0xbd, 0xd4),
        // chips
        hsl(266, 0.35, 0.93), hsl(266, 0.35, 0.93).darker(104), hsl(266, 0.35, 0.93).darker(110),
        // brand
        QColor(154, 81, 248), hsl(266, 0.80, 0.50),
        // controls
        hsl(266, 0.30, 0.88), hsl(266, 1.00, 0.92), QColor(0xf6, 0xf2, 0xfd), QColor(0xf9, 0xf6, 0xfe),
        QColor(0xef, 0xec, 0xf4), QColor(0xb3, 0xad, 0xc4), QColor(0x1a, 0x0f, 0x33), stageText,
        // tints
        {
            { QColor(154, 81, 248), hsl(266, 1.00, 0.96) }, // violet
            { hsl(250, 0.92, 0.56), hsl(250, 1.00, 0.96) }, // indigo
            { QColor(0xb4, 0x53, 0x09), QColor(0xff, 0xf4, 0xdc) }, // amber (its text shade on light)
            { hsl(289, 0.75, 0.52), hsl(289, 1.00, 0.96) }, // magenta
        },
    };
    return c;
}

const Colors& dark()
{
    static const Colors c {
        // neutrals
        QColor(0x07, 0x06, 0x0d), QColor(0x13, 0x11, 0x1f), QColor(0x1c, 0x19, 0x30),
        QColor(0xf1, 0xed, 0xf9), QColor(0xa9, 0xa2, 0xbf), QColor(0x8a, 0x83, 0xa1),
        hsl(266, 0.20, 0.22), QColor(0x4a, 0x44, 0x60),
        // chips: translucent white
        QColor(255, 255, 255, 31), QColor(255, 255, 255, 46), QColor(255, 255, 255, 64),
        // brand
        hsl(266, 1.00, 0.68), hsl(266, 1.00, 0.76),
        // controls
        hsl(266, 0.20, 0.26), hsl(266, 0.45, 0.30), QColor(0x24, 0x1f, 0x3a), QColor(0x21, 0x1d, 0x38),
        QColor(0x1a, 0x17, 0x26), QColor(0x5a, 0x54, 0x70), QColor(0x2a, 0x25, 0x44), stageText,
        // tints
        {
            { hsl(266, 1.00, 0.68), hsl(266, 0.40, 0.14) }, // violet
            { hsl(250, 0.92, 0.70), hsl(250, 0.40, 0.14) }, // indigo
            { QColor(0xf5, 0x9e, 0x0b), hsl(35, 0.45, 0.11) }, // amber
            { hsl(289, 0.75, 0.66), hsl(289, 0.40, 0.13) }, // magenta
        },
    };
    return c;
}

QFont font(qreal pointSize, QFont::Weight weight)
{
    // --rui-font-brand
    QFont f(QStringList { QStringLiteral("Inter"), QStringLiteral("Segoe UI"), QStringLiteral("Roboto"), QStringLiteral("DejaVu Sans") });
    f.setPointSizeF(pointSize);
    f.setWeight(weight);
    return f;
}

} // namespace Theme

} // namespace revolut
