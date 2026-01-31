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
namespace flat {
ThemeEntry themeEntry();
}
namespace claudecode {
ThemeEntry themeEntry();
}
namespace vscode {
ThemeEntry themeEntry();
}
namespace proxmox {
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
        flat::themeEntry(),
        claudecode::themeEntry(),
        vscode::themeEntry(),
        proxmox::themeEntry(),
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

bool savedDark()
{
    return QSettings().value(QStringLiteral("dark"), false).toBool();
}

void saveDark(bool dark)
{
    QSettings().setValue(QStringLiteral("dark"), dark);
}

} // namespace Themes
