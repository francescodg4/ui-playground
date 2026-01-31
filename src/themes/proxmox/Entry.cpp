#include "ThemeRegistry.hpp"
#include "themes/proxmox/Style.hpp"

#include <QCoreApplication>

namespace proxmox {

ThemeEntry themeEntry()
{
    return {
        QStringLiteral("proxmox"),
        QCoreApplication::translate("Themes", "Proxmox"),
        QCoreApplication::translate("Themes", "Proxmox VE console: square panels with blue titles, solid-blue primary actions and tabs, thin usage bars; light or dark."),
        []() -> WidgetStyle* { return new Style(false); },
        []() -> WidgetStyle* { return new Style(true); },
    };
}

} // namespace proxmox
