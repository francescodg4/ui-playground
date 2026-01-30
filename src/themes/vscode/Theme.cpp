#include "themes/vscode/Theme.hpp"

#include <QStringList>

namespace vscode {

namespace Theme {

QFont font(qreal pointSize, QFont::Weight weight)
{
    // --font-ui, with the Linux stack's fallbacks
    QFont f(QStringList { QStringLiteral("Segoe UI"), QStringLiteral("system-ui"), QStringLiteral("Ubuntu"), QStringLiteral("Droid Sans"), QStringLiteral("DejaVu Sans") });
    f.setPointSizeF(pointSize);
    f.setWeight(weight);
    return f;
}

} // namespace Theme

} // namespace vscode
