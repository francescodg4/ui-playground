#include "ThemeRegistry.hpp"
#include "themes/glass/PdaWindow.hpp"
#include "themes/glass/Theme.hpp"
#include "themes/glass/widgets/Glass.hpp"

#include <QCoreApplication>
#include <QFontDatabase>

namespace glass {

ThemeEntry themeEntry()
{
    return {
        QStringLiteral("glass"),
        QCoreApplication::translate("Themes", "Liquid Glass"),
        QCoreApplication::translate("Themes", "Frosted glass over a live ambient canvas, bento cards, clay controls and radiant highlights."),
        &Theme::styleSheet,
        [] {
            QFont font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
            font.setPointSizeF(10);
            return font;
        },
        [](PhotoLibrary* photos, const ThemeOptions& options) -> PdaShell* {
            Glass::settings().reducedMotion = options.reducedMotion;
            Glass::settings().reducedTransparency = options.reducedTransparency;
            return new PdaWindow(photos);
        },
    };
}

} // namespace glass
