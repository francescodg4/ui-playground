#include "ThemeRegistry.hpp"

#include <QSettings>

// each theme registers itself through one factory function
namespace glass {
ThemeEntry themeEntry();
}
namespace emerald {
ThemeEntry themeEntry();
}
namespace winamp {
ThemeEntry themeEntry();
}
namespace metro {
ThemeEntry themeEntry();
}
namespace revolut {
ThemeEntry themeEntry();
}

namespace Themes {

const QList<ThemeEntry>& all()
{
    static const QList<ThemeEntry> themes = {
        glass::themeEntry(),
        emerald::themeEntry(),
        winamp::themeEntry(),
        metro::themeEntry(),
        revolut::themeEntry(),
    };
    return themes;
}

const ThemeEntry* find(const QString& id)
{
    for (const ThemeEntry& theme : all()) {
        if (theme.id == id) {
            return &theme;
        }
    }
    return nullptr;
}

QString saved()
{
    const QString id = QSettings().value(QStringLiteral("theme")).toString();
    return find(id) ? id : all().first().id;
}

void save(const QString& id)
{
    QSettings().setValue(QStringLiteral("theme"), id);
}

} // namespace Themes
