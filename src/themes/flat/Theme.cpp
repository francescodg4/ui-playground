#include "themes/flat/Theme.hpp"

#include <QStringList>

namespace flat {

namespace Theme {

QFont font(qreal pointSize, QFont::Weight weight)
{
    // --flat-font-ui
    QFont f(QStringList { QStringLiteral("Segoe UI"), QStringLiteral("Noto Sans"), QStringLiteral("DejaVu Sans") });
    f.setPointSizeF(pointSize);
    f.setWeight(weight);
    return f;
}

QFont mono(qreal pointSize)
{
    // --flat-font-mono
    QFont f(QStringList { QStringLiteral("Consolas"), QStringLiteral("DejaVu Sans Mono") });
    f.setStyleHint(QFont::Monospace);
    f.setPointSizeF(pointSize);
    return f;
}

} // namespace Theme

} // namespace flat
