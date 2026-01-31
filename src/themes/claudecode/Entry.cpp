#include "ThemeRegistry.hpp"
#include "themes/claudecode/Style.hpp"

#include <QCoreApplication>

namespace claudecode {

ThemeEntry themeEntry()
{
    return {
        QStringLiteral("claudecode"),
        QCoreApplication::translate("Themes", "Claude Code"),
        QCoreApplication::translate("Themes", "Dark IDE chrome in warm neutrals, rich text on cream paper, and a single coral accent."),
        []() -> WidgetStyle* { return new Style; },
        nullptr,
    };
}

} // namespace claudecode
