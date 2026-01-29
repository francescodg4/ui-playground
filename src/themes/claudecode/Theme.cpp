#include "themes/claudecode/Theme.hpp"

#include <QStringList>

namespace claudecode {

namespace Theme {

QFont font(qreal pointSize, QFont::Weight weight)
{
    // --font-ui
    QFont f(QStringList { QStringLiteral("Segoe UI"), QStringLiteral("Inter"), QStringLiteral("DejaVu Sans") });
    f.setPointSizeF(pointSize);
    f.setWeight(weight);
    return f;
}

} // namespace Theme

} // namespace claudecode
