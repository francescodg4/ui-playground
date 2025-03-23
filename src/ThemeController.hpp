#pragma once

#include "ThemeRegistry.hpp"

#include <QObject>
#include <QPointer>

class PdaShell;
class PhotoLibrary;
class WidgetGallery;

/// Owns the main window and rebuilds it when the user selects another theme: the new window
/// takes over the geometry, window state and current page of the old one. It also swaps the PDA
/// for the widget gallery, where the interface is chosen on the standard widgets, and back.
class ThemeController : public QObject {
    Q_OBJECT
public:
    ThemeController(PhotoLibrary* photos, const ThemeOptions& options, QObject* parent = nullptr);

    PdaShell* window() const { return m_window; }
    WidgetGallery* gallery() const { return m_gallery; }
    QString current() const { return m_current; }

    /// Builds and shows the window of theme @p id (keeps the current one if the id is unknown).
    bool apply(const QString& id);
    /// Opens the theme selector; the choice is applied and remembered.
    void selectTheme();
    /// Replaces the PDA with the widget gallery, in theme @p id (default: the current one).
    void showGallery(const QString& id = {});

    /// Disables theme animations (screenshots).
    void setAnimationsEnabled(bool enabled);

private:
    /// Closes the gallery and shows the PDA in theme @p id.
    void leaveGallery(const QString& id);

    PhotoLibrary* m_photos;
    ThemeOptions m_options;
    QPointer<PdaShell> m_window;
    QPointer<WidgetGallery> m_gallery;
    QString m_current;
    bool m_animations = true;
};
