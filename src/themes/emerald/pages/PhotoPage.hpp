#pragma once

#include <QHash>
#include <QImage>
#include <QWidget>

class PhotoLibrary;
class QListWidget;
class QStackedWidget;

namespace emerald {

class DataScreen;
class PixelKey;
class PixelLabel;

/// Box of acquired photos with a data window preview, and a full-size viewer
/// (previous / next / delete / back).
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
    void select(int index);
    void importPhotos();
    void deleteCurrent();
    QImage thumbnail(int index);

    PhotoLibrary* m_library;
    QStackedWidget* m_views = nullptr;
    QListWidget* m_grid = nullptr;
    DataScreen* m_preview = nullptr;
    PixelLabel* m_info = nullptr;
    PixelLabel* m_count = nullptr;
    PixelKey* m_view = nullptr;

    QWidget* m_viewer = nullptr;
    DataScreen* m_screen = nullptr;
    PixelLabel* m_counter = nullptr;
    PixelKey* m_prev = nullptr;
    PixelKey* m_next = nullptr;
    int m_selected = -1;
    int m_current = -1;

    QHash<QString, QImage> m_thumbnails; ///< pixelated, keyed by path + modification time
};

} // namespace emerald
