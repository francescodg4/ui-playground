#include "ThemeRegistry.hpp"
#include "themes/vscode/Style.hpp"

#include <QCoreApplication>

namespace vscode {

ThemeEntry themeEntry()
{
    return {
        QStringLiteral("vscode"),
        QCoreApplication::translate("Themes", "Visual Studio Code"),
        QCoreApplication::translate("Themes", "Dark+: flat grey chrome with 1px borders, 22px rows, lightly rounded controls and one blue accent."),
        []() -> WidgetStyle* { return new Style; },
    };
}

} // namespace vscode
