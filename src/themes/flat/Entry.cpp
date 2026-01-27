#include "ThemeRegistry.hpp"
#include "themes/flat/Style.hpp"

#include <QCoreApplication>

namespace flat {

ThemeEntry themeEntry()
{
    return {
        QStringLiteral("flat"),
        QCoreApplication::translate("Themes", "Flat"),
        QCoreApplication::translate("Themes", "Dark operations console: navy hairline panels on black, muted labels and light values, docked tabs, one signal green."),
        []() -> WidgetStyle* { return new Style; },
    };
}

} // namespace flat
