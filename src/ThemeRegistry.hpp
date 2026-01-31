#pragma once

#include <QList>
#include <QString>

class WidgetStyle;

/// One selectable design of the interface.
struct ThemeEntry {
    QString id; ///< used on the command line and in the settings
    QString name;
    QString description;
    WidgetStyle* (*widgetStyle)(); ///< the standard widgets in this design
    WidgetStyle* (*darkStyle)(); ///< its dark mode, or null when the design has none
};

namespace Themes {

const QList<ThemeEntry>& all();
const ThemeEntry* find(const QString& id);

/// The theme chosen last time (or the first one).
QString saved();
void save(const QString& id);

/// Whether dark mode was on last time.
bool savedDark();
void saveDark(bool dark);

} // namespace Themes
