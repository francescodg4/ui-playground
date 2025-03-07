#pragma once

#include "ThemeRegistry.hpp"

#include <QObject>
#include <QPointer>

class PdaShell;
class PhotoLibrary;

/// Owns the main window and rebuilds it when the user selects another theme: the new window
/// takes over the geometry, window state and current page of the old one.
class ThemeController : public QObject {
    Q_OBJECT
public:
    ThemeController(PhotoLibrary* photos, const ThemeOptions& options, QObject* parent = nullptr);

    PdaShell* window() const { return m_window; }
    QString current() const { return m_current; }

    /// Builds and shows the window of theme @p id (keeps the current one if the id is unknown).
    bool apply(const QString& id);
    /// Opens the theme selector; the choice is applied and remembered.
    void selectTheme();

    /// Disables theme animations (screenshots).
    void setAnimationsEnabled(bool enabled);

private:
    PhotoLibrary* m_photos;
    ThemeOptions m_options;
    QPointer<PdaShell> m_window;
    QString m_current;
    bool m_animations = true;
};
