#pragma once

#include <QFont>
#include <QList>
#include <QString>

class PdaShell;
class PhotoLibrary;

/// Accessibility switches passed to every theme.
struct ThemeOptions {
    bool reducedMotion = false;
    bool reducedTransparency = false;
};

/// One selectable design of the interface.
struct ThemeEntry {
    QString id; ///< used on the command line and in the settings
    QString name;
    QString description;
    QString (*styleSheet)();
    QFont (*font)();
    PdaShell* (*create)(PhotoLibrary* photos, const ThemeOptions& options);
};

namespace Themes {

const QList<ThemeEntry>& all();
const ThemeEntry* find(const QString& id);

/// The theme chosen last time (or the first one).
QString saved();
void save(const QString& id);

} // namespace Themes
