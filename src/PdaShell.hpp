#pragma once

#include <QWidget>

/// What every theme's main window offers the application: page navigation, animation control
/// and a way to ask for the theme selector.
class PdaShell : public QWidget {
    Q_OBJECT
public:
    using QWidget::QWidget;

    virtual int pageCount() const = 0;
    virtual int currentPage() const = 0;
    virtual void setPage(int index) = 0;
    /// Opens the photo viewer on the first photo (used by screenshot mode).
    virtual void showPhotoViewer() = 0;
    /// Animations that are part of the theme (off for reduced motion and screenshots).
    virtual void setAnimationsEnabled(bool) { }

signals:
    /// The user asked to pick another theme ("Select theme" control or Ctrl+T).
    void selectThemeRequested();
};
