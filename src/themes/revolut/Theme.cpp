#include "themes/revolut/Theme.hpp"

#include <QStringList>

namespace revolut {

namespace Theme {

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
