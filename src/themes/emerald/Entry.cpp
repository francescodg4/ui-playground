#include "ThemeRegistry.hpp"
#include "themes/emerald/PdaWindow.hpp"
#include "themes/emerald/Style.hpp"
#include "themes/emerald/Theme.hpp"

#include <QCoreApplication>

namespace emerald {

ThemeEntry themeEntry()
{
    return {
        QStringLiteral("emerald"),
        QCoreApplication::translate("Themes", "Emerald"),
        QCoreApplication::translate("Themes", "GBA-era storage box: pixel grid, box wallpapers, data windows, capsule keys and the glove cursor."),
        &Theme::styleSheet,
        [] { return QFont(QStringLiteral("DejaVu Sans Mono"), 9); },
        [](PhotoLibrary* photos, const ThemeOptions&) -> PdaShell* { return new PdaWindow(photos); },
        []() -> WidgetStyle* { return new Style; },
    };
}

} // namespace emerald
