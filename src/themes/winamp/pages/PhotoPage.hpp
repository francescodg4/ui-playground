#pragma once

#include <QHash>
#include <QPixmap>
#include <QTimer>
#include <QWidget>

class PhotoLibrary;
class QLabel;
class QListWidget;
class QStackedWidget;

namespace winamp {

class CapsuleButton;
class DockTabs;
class PhotoView;
class PositionSlider;
class RoundButton;

/// Gallery and viewer modules docked under bottom tabs. The viewer is driven like a player:
/// position slider, transport deck (previous, slideshow play / pause / stop, next) and
/// capsules for import (eject), gallery (PL) and delete.
class PhotoPage : public QWidget {
    Q_OBJECT
public:
    explicit PhotoPage(PhotoLibrary* library, QWidget* parent = nullptr);

    void openPhoto(int index);
    void closeViewer();
    bool isViewerOpen() const;

private:
    QWidget* buildGallery();
    QWidget* buildViewer();
    void rebuild();
    void importPhotos();
    void deleteCurrent();
    void setSlideshow(bool running);
    QPixmap thumbnail(int index);

    PhotoLibrary* m_library;
    QStackedWidget* m_views = nullptr;
    DockTabs* m_tabs = nullptr;
    QListWidget* m_grid = nullptr;
    QLabel* m_count = nullptr;

    QWidget* m_viewer = nullptr;
    PhotoView* m_view = nullptr;
    PositionSlider* m_position = nullptr;
    RoundButton* m_play = nullptr;
    RoundButton* m_prev = nullptr;
    RoundButton* m_next = nullptr;
    QTimer m_slideshow;
    int m_current = -1;

    QHash<QString, QPixmap> m_thumbnails; ///< keyed by path + modification time
};

} // namespace winamp
