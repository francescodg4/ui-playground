#include "themes/metro/Theme.hpp"

#include <QStringList>

namespace metro {

namespace Theme {

QFont font(qreal pointSize, QFont::Weight weight)
{
    // --metro-font-family
    QFont f(QStringList { QStringLiteral("Segoe UI"), QStringLiteral("Helvetica Neue"), QStringLiteral("Arial"), QStringLiteral("DejaVu Sans") });
    f.setPointSizeF(pointSize);
    f.setWeight(weight);
    return f;
}

} // namespace Theme

} // namespace metro
