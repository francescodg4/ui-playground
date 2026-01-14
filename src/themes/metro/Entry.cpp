#include "ThemeRegistry.hpp"
#include "themes/metro/Style.hpp"

#include <QCoreApplication>

namespace metro {

ThemeEntry themeEntry()
{
    return {
        QStringLiteral("metro"),
        QCoreApplication::translate("Themes", "Metro"),
        QCoreApplication::translate("Themes", "Windows 8 Modern UI: flat and square on a deep canvas, typography instead of boxes, accent live tiles."),
        []() -> WidgetStyle* { return new Style; },
    };
}

} // namespace metro
