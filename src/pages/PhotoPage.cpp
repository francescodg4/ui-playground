#include "PhotoPage.hpp"

#include "Icons.hpp"
#include "PhotoLibrary.hpp"
#include "Theme.hpp"
#include "widgets/Holo.hpp"

#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QShortcut>
#include <QStackedWidget>
#include <QToolButton>

#include <functional>

namespace {
const QSize ThumbSize(160, 90);
const QSize FilmSize(72, 40);

/// Scales @p image to fill @p size and crops the overflow (like CSS object-fit: cover).
QPixmap cover(const QImage& image, const QSize& size)
{
    if (image.isNull()) {
        QPixmap empty(size);
        empty.fill(QColor(0, 30, 60, 120));
        return empty;
    }
    const QImage scaled = image.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    const QRect crop(QPoint((scaled.width() - size.width()) / 2, (scaled.height() - size.height()) / 2), size);
    return QPixmap::fromImage(scaled.copy(crop));
}

/// Dark rounded panel behind the viewer.
class ViewerPanel : public QWidget {
protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QPainterPath shape;
        shape.addRoundedRect(QRectF(rect()).adjusted(1, 1, -1, -1), 14, 14);
        p.fillPath(shape, QColor(6, 40, 75, 225));
        p.strokePath(shape, QPen(Theme::line, 1.5));
    }
};

QToolButton* navButton(Icon icon, const QString& toolTip)
{
    auto* button = new QToolButton;
    button->setObjectName(QStringLiteral("nav"));
    button->setIcon(Icons::icon(icon));
    button->setIconSize(QSize(22, 22));
    button->setFixedSize(42, 76);
    button->setToolTip(toolTip);
    button->setCursor(Qt::PointingHandCursor);
    button->setFocusPolicy(Qt::NoFocus);
    return button;
}

} // namespace

/// Shows one photo scaled to fit, with a soft glow; horizontal swipes step through photos.
class PhotoView : public QWidget {
public:
    std::function<void(int)> onSwipe; ///< -1 previous, +1 next

    void setPhoto(const QPixmap& pixmap)
    {
        m_pixmap = pixmap;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        if (m_pixmap.isNull()) {
            return;
        }
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        const QSizeF size = QSizeF(m_pixmap.size()).scaled(QSizeF(this->size()) - QSizeF(16, 16), Qt::KeepAspectRatio);
        const QRectF target(QPointF((width() - size.width()) / 2, (height() - size.height()) / 2), size);
        for (int i = 4; i >= 1; --i) {
            p.setPen(QPen(QColor(100, 200, 255, 22), i * 3));
            p.drawRect(target);
        }
        p.drawPixmap(target, m_pixmap, QRectF(m_pixmap.rect()));
        p.setPen(QPen(QColor(220, 245, 255, 150), 1));
        p.drawRect(target);
    }

    void mousePressEvent(QMouseEvent* e) override { m_pressX = e->position().x(); }
    void mouseReleaseEvent(QMouseEvent* e) override
    {
        const qreal dx = e->position().x() - m_pressX;
        if (std::abs(dx) > 60 && onSwipe) {
            onSwipe(dx < 0 ? 1 : -1);
        }
    }

private:
    QPixmap m_pixmap;
    qreal m_pressX = 0;
};

PhotoPage::PhotoPage(PhotoLibrary* library, QWidget* parent)
    : QWidget(parent)
    , m_library(library)
{
    m_views = new QStackedWidget;
    m_views->addWidget(buildGallery());
    m_views->addWidget(buildViewer());

    auto* layout = Holo::pageLayout(this, tr("Photo Manager"));
    layout->addWidget(m_views, 1);

    connect(m_library, &PhotoLibrary::changed, this, &PhotoPage::rebuild);
    rebuild();
}

QWidget* PhotoPage::buildGallery()
{
    auto* gallery = new QWidget;

    m_count = new QLabel;
    m_count->setObjectName(QStringLiteral("dim"));
    m_count->setToolTip(QDir::toNativeSeparators(m_library->directory()));
    auto* import = new QPushButton(Icons::icon(Icon::Plus), tr("Import"));
    import->setObjectName(QStringLiteral("chip"));
    import->setIconSize(QSize(14, 14));
    import->setCursor(Qt::PointingHandCursor);
    import->setFocusPolicy(Qt::NoFocus);
    connect(import, &QPushButton::clicked, this, &PhotoPage::importPhotos);

    m_grid = new QListWidget;
    m_grid->setObjectName(QStringLiteral("gallery"));
    m_grid->setViewMode(QListView::IconMode);
    m_grid->setIconSize(ThumbSize);
    m_grid->setSpacing(10);
    m_grid->setMovement(QListView::Static);
    m_grid->setResizeMode(QListView::Adjust);
    m_grid->setUniformItemSizes(true);
    m_grid->setSelectionMode(QAbstractItemView::NoSelection);
    m_grid->setFocusPolicy(Qt::NoFocus);
    m_grid->setMouseTracking(true);
    m_grid->viewport()->setAutoFillBackground(false);
    m_grid->setCursor(Qt::PointingHandCursor);
    connect(m_grid, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) { openPhoto(m_grid->row(item)); });

    m_empty = new QLabel(tr("No photos acquired yet."));
    m_empty->setObjectName(QStringLiteral("dim"));
    m_empty->setAlignment(Qt::AlignCenter);

    auto* bar = new QHBoxLayout;
    bar->addWidget(m_count);
    bar->addStretch();
    bar->addWidget(import);
    bar->addSpacing(16);

    auto* layout = new QVBoxLayout(gallery);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(bar);
    layout->addWidget(m_grid, 1);
    layout->addWidget(m_empty, 1);
    return gallery;
}

QWidget* PhotoPage::buildViewer()
{
    m_viewer = new ViewerPanel;
    m_viewer->setFocusPolicy(Qt::StrongFocus);

    m_name = new QLabel;
    m_name->setStyleSheet(QStringLiteral("font-weight: bold;"));
    m_meta = new QLabel;
    m_meta->setObjectName(QStringLiteral("dim"));
    auto* remove = Holo::roundButton(Icon::Trash, tr("Delete photo (Del)"));
    auto* close = Holo::roundButton(Icon::Close, tr("Close (Esc)"));

    m_prev = navButton(Icon::ChevronLeft, tr("Previous (Left)"));
    m_next = navButton(Icon::ChevronRight, tr("Next (Right)"));
    m_view = new PhotoView;
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_view->onSwipe = [this](int step) { openPhoto(m_current + step); };

    m_filmstrip = new QListWidget;
    m_filmstrip->setObjectName(QStringLiteral("filmstrip"));
    m_filmstrip->setViewMode(QListView::IconMode);
    m_filmstrip->setWrapping(false);
    m_filmstrip->setFlow(QListView::LeftToRight);
    m_filmstrip->setMovement(QListView::Static);
    m_filmstrip->setIconSize(FilmSize);
    m_filmstrip->setSpacing(4);
    m_filmstrip->setFixedHeight(FilmSize.height() + 26);
    m_filmstrip->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_filmstrip->setFocusPolicy(Qt::NoFocus);
    m_filmstrip->viewport()->setAutoFillBackground(false);
    connect(m_filmstrip, &QListWidget::currentRowChanged, this, &PhotoPage::openPhoto);

    auto* top = new QHBoxLayout;
    top->addWidget(m_name);
    top->addSpacing(10);
    top->addWidget(m_meta, 1);
    top->addWidget(remove);
    top->addWidget(close);

    auto* stage = new QHBoxLayout;
    stage->addWidget(m_prev);
    stage->addWidget(m_view, 1);
    stage->addWidget(m_next);

    auto* layout = new QVBoxLayout(m_viewer);
    layout->setContentsMargins(12, 10, 12, 8);
    layout->addLayout(top);
    layout->addLayout(stage, 1);
    layout->addWidget(m_filmstrip);

    connect(m_prev, &QToolButton::clicked, this, [this] { openPhoto(m_current - 1); });
    connect(m_next, &QToolButton::clicked, this, [this] { openPhoto(m_current + 1); });
    connect(close, &QToolButton::clicked, this, &PhotoPage::closeViewer);
    connect(remove, &QToolButton::clicked, this, &PhotoPage::deleteCurrent);

    const auto shortcut = [this](QKeySequence key, auto slot) {
        auto* s = new QShortcut(key, m_viewer);
        s->setContext(Qt::WidgetWithChildrenShortcut);
        connect(s, &QShortcut::activated, this, slot);
    };
    shortcut(QKeySequence(Qt::Key_Left), [this] { openPhoto(m_current - 1); });
    shortcut(QKeySequence(Qt::Key_Right), [this] { openPhoto(m_current + 1); });
    shortcut(QKeySequence(Qt::Key_Escape), [this] { closeViewer(); });
    shortcut(QKeySequence(Qt::Key_Delete), [this] { deleteCurrent(); });
    return m_viewer;
}

QPixmap PhotoPage::thumbnail(int index)
{
    const Photo& photo = m_library->photos().at(index);
    const QString key = photo.path + QString::number(photo.acquired.toMSecsSinceEpoch());
    auto it = m_thumbnails.find(key);
    if (it == m_thumbnails.end()) {
        it = m_thumbnails.insert(key, cover(QImage(photo.path), ThumbSize));
    }
    return *it;
}

void PhotoPage::rebuild()
{
    const QList<Photo>& photos = m_library->photos();
    m_count->setText(photos.size() == 1 ? tr("1 photo") : tr("%1 photos").arg(photos.size()));
    m_grid->setVisible(!photos.isEmpty());
    m_empty->setVisible(photos.isEmpty());

    m_grid->clear();
    m_filmstrip->blockSignals(true);
    m_filmstrip->clear();
    for (int i = 0; i < photos.size(); ++i) {
        const QPixmap thumb = thumbnail(i);
        const QString tip = QStringLiteral("%1\n%2").arg(photos[i].name, photos[i].acquired.toString(QStringLiteral("yyyy-MM-dd HH:mm")));
        auto* item = new QListWidgetItem(QIcon(thumb), QString(), m_grid);
        item->setSizeHint(ThumbSize + QSize(2, 2));
        item->setToolTip(tip);
        auto* film = new QListWidgetItem(QIcon(thumb), QString(), m_filmstrip);
        film->setSizeHint(FilmSize + QSize(4, 4));
        film->setToolTip(photos[i].name);
    }
    m_filmstrip->blockSignals(false);

    if (isViewerOpen()) {
        if (photos.isEmpty()) {
            closeViewer();
        } else {
            const int keep = std::min(m_current, int(photos.size()) - 1);
            m_current = -1; // force a reload: the file at this index may have changed
            openPhoto(keep);
        }
    }
}

void PhotoPage::openPhoto(int index)
{
    const QList<Photo>& photos = m_library->photos();
    if (index < 0 || index >= photos.size()) {
        return;
    }
    if (index != m_current || !isViewerOpen()) {
        m_current = index;
        const Photo& photo = photos[index];
        m_view->setPhoto(QPixmap(photo.path));
        m_name->setText(photo.name);
        m_meta->setText(QStringLiteral("%1 / %2   ·   %3").arg(index + 1).arg(photos.size()).arg(photo.acquired.toString(QStringLiteral("yyyy-MM-dd HH:mm"))));
        m_prev->setEnabled(index > 0);
        m_next->setEnabled(index < photos.size() - 1);
        const QSignalBlocker block(m_filmstrip);
        m_filmstrip->setCurrentRow(index);
        m_filmstrip->scrollToItem(m_filmstrip->item(index), QAbstractItemView::PositionAtCenter);
    }
    m_views->setCurrentWidget(m_viewer);
    m_viewer->setFocus();
}

void PhotoPage::closeViewer()
{
    m_views->setCurrentIndex(0);
    m_current = -1;
}

bool PhotoPage::isViewerOpen() const
{
    return m_views->currentWidget() == m_viewer;
}

void PhotoPage::importPhotos()
{
    const QStringList files = QFileDialog::getOpenFileNames(this, tr("Import photos"), QDir::homePath(),
        tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));
    if (!files.isEmpty()) {
        m_library->import(files);
    }
}

void PhotoPage::deleteCurrent()
{
    if (!isViewerOpen() || m_current < 0) {
        return;
    }
    const Photo photo = m_library->photos().at(m_current);
    if (QMessageBox::question(this, tr("Delete photo"), tr("Delete \"%1\" from the device?").arg(photo.name)) == QMessageBox::Yes) {
        m_library->remove(m_current);
    }
}
