#pragma once

#include <QHash>
#include <QPixmap>
#include <QWidget>

class PhotoLibrary;
class PhotoView;
class QLabel;
class QListWidget;
class QStackedWidget;
class ClayButton;
class ViewerPanel;

/// Gallery of the acquired images with a full-size viewer (previous / next / delete).
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
    QPixmap thumbnail(int index);

    PhotoLibrary* m_library;
    QStackedWidget* m_views = nullptr;
    QListWidget* m_grid = nullptr;
    QLabel* m_count = nullptr;
    QLabel* m_empty = nullptr;

    ViewerPanel* m_viewer = nullptr;
    QWidget* m_gallery = nullptr;
    PhotoView* m_view = nullptr;
    QLabel* m_name = nullptr;
    QLabel* m_meta = nullptr;
    ClayButton* m_prev = nullptr;
    ClayButton* m_next = nullptr;
    QListWidget* m_filmstrip = nullptr;
    int m_current = -1;

    QHash<QString, QPixmap> m_thumbnails; ///< keyed by path + modification time
};
