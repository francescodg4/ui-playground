#include "ThemeRegistry.hpp"
#include "themes/revolut/Style.hpp"

#include <QCoreApplication>

namespace revolut {

ThemeEntry themeEntry()
{
    return {
        QStringLiteral("revolut"),
        QCoreApplication::translate("Themes", "Revolut"),
        QCoreApplication::translate("Themes", "Fintech app widgets in violet: tinted cards with one accent each, pill buttons, an amber focus ring and dark stages."),
        []() -> WidgetStyle* { return new Style; },
    };
}

} // namespace revolut
