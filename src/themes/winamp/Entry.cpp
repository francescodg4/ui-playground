#include "ThemeRegistry.hpp"
#include "themes/winamp/PdaWindow.hpp"
#include "themes/winamp/Style.hpp"
#include "themes/winamp/Theme.hpp"
#include "themes/winamp/widgets/Metal.hpp"

#include <QCoreApplication>

namespace winamp {

ThemeEntry themeEntry()
{
    return {
        QStringLiteral("winamp"),
        QCoreApplication::translate("Themes", "Winamp"),
        QCoreApplication::translate("Themes", "Winamp Modern skin: brushed metal, cobalt LCD telemetry with a dot-matrix status line and visualizer, transport controls."),
        &Theme::styleSheet,
        [] { return Metal::uiFont(9); },
        [](PhotoLibrary* photos, const ThemeOptions&) -> PdaShell* { return new PdaWindow(photos); },
        []() -> WidgetStyle* { return new Style; },
    };
}

} // namespace winamp
