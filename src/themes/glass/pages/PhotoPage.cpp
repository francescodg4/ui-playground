#include "PhotoPage.hpp"

#include "Icons.hpp"
#include "PhotoLibrary.hpp"
#include "themes/glass/Theme.hpp"
#include "themes/glass/widgets/ClayButton.hpp"
#include "themes/glass/widgets/Glass.hpp"
#include "themes/glass/widgets/Holo.hpp"

#include <QDir>
#include <QFileDialog>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QShortcut>
#include <QStackedWidget>
#include <QStyledItemDelegate>

#include <functional>

namespace glass {

namespace {
const QSize ThumbSize(160, 90);
const QSize FilmSize(72, 40);

/// Scales @p image to fill @p size, crops the overflow (like CSS object-fit: cover) and rounds the corners.
QPixmap cover(const QImage& image, const QSize& size)
{
    QPixmap result(size);
    result.fill(Qt::transparent);
    QPainter p(&result);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    QPainterPath clip;
    clip.addRoundedRect(QRectF(QPointF(), QSizeF(size)), 10, 10);
    if (image.isNull()) {
        p.fillPath(clip, Theme::solidSurface);
        return result;
    }
    const QImage scaled = image.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    p.setClipPath(clip);
    p.drawImage(QPointF((size.width() - scaled.width()) / 2.0, (size.height() - scaled.height()) / 2.0), scaled);
    return result;
}

/// Draws item thumbnails as rounded tiles: hover and selection radiate a glow, a press
/// compresses the tile.
class ThumbDelegate : public QStyledItemDelegate {
public:
    ThumbDelegate(const QSize& thumb, int pad, qreal radius, qreal glow, QObject* parent)
        : QStyledItemDelegate(parent)
        , m_thumb(thumb)
        , m_pad(pad)
        , m_radius(radius)
        , m_glow(glow)
    {
    }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        return m_thumb + QSize(2 * m_pad, 2 * m_pad);
    }

    void paint(QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        const QRectF r = QRectF(option.rect).adjusted(m_pad, m_pad, -m_pad, -m_pad);
        const bool hover = option.state & QStyle::State_MouseOver;
        const bool selected = option.state & QStyle::State_Selected;
        const bool pressed = hover && (QGuiApplication::mouseButtons() & Qt::LeftButton);

        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        p->setRenderHint(QPainter::SmoothPixmapTransform);
        if (selected) {
            Glass::paintGlow(*p, r, m_radius, Theme::glowAccent, m_glow);
        } else if (hover) {
            Glass::paintGlow(*p, r, m_radius, Theme::glowSoft, 0.8 * m_glow);
        }
        const QRectF face = pressed ? Glass::scaled(r, Theme::pressedScale) : r;
        QPainterPath shape;
        shape.addRoundedRect(face, m_radius, m_radius);
        p->setClipPath(shape);
        p->drawPixmap(face, index.data(Qt::DecorationRole).value<QIcon>().pixmap(m_thumb), QRectF(0, 0, m_thumb.width(), m_thumb.height()));
        p->setClipping(false);
        QLinearGradient light(face.topLeft(), face.bottomRight());
        light.setColorAt(0, Theme::glassHighlight);
        light.setColorAt(0.45, Theme::glassBorder);
        p->strokePath(shape, QPen(QBrush(light), 1));
        p->restore();
    }

private:
    QSize m_thumb;
    int m_pad;
    qreal m_radius;
    qreal m_glow; ///< glow strength: small tiles have little room around them
};

ClayButton* navButton(Icon icon, const QString& toolTip)
{
    auto* button = new ClayButton(icon);
    button->setBodySize(QSize(40, 72));
    button->setToolTip(toolTip);
    return button;
}

} // namespace

/// Liquid-glass overlay: blurs the gallery it covers (and the canvas) at the highest elevation.
class ViewerPanel : public QWidget {
public:
    /// Snapshot of what lies underneath, in host coordinates.
    void setUnderlay(const QImage& blurred, const QRect& hostRect)
    {
        m_underlay = blurred;
        m_underlayRect = hostRect;
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        const QRectF r = QRectF(rect());
        Glass::paintBackdrop(p, this, r, Theme::radiusCard, Glass::Level::Modal);
        if (QWidget* host = Glass::Backdrop::instance().hostFor(this); host && !m_underlay.isNull() && !Glass::settings().reducedTransparency) {
            QPainterPath shape;
            shape.addRoundedRect(r, Theme::radiusCard, Theme::radiusCard);
            p.save();
            p.setClipPath(shape);
            p.drawImage(QRectF(m_underlayRect.translated(-mapTo(host, QPoint(0, 0)))), m_underlay);
            p.restore();
        }
        Glass::paintTint(p, this, r, Theme::radiusCard, Glass::Level::Modal);
    }

private:
    QImage m_underlay;
    QRect m_underlayRect;
};

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
        const QSizeF size = QSizeF(m_pixmap.size()).scaled(QSizeF(this->size()) - QSizeF(24, 24), Qt::KeepAspectRatio);
        const QRectF target(QPointF((width() - size.width()) / 2, (height() - size.height()) / 2), size);
        QPainterPath clip;
        clip.addRoundedRect(target, Theme::radiusControl, Theme::radiusControl);
        Glass::paintGlow(p, target, Theme::radiusControl, QColor(0, 0, 0, 90));
        p.setClipPath(clip);
        p.drawPixmap(target, m_pixmap, QRectF(m_pixmap.rect()));
        p.setClipping(false);
        QLinearGradient light(target.topLeft(), target.bottomRight());
        light.setColorAt(0, Theme::glassHighlight);
        light.setColorAt(0.45, Theme::glassBorder);
        p.strokePath(clip, QPen(QBrush(light), 1));
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
    auto* gallery = new Glass::GlassCard;
    m_gallery = gallery;

    m_count = new QLabel;
    m_count->setObjectName(QStringLiteral("dim"));
    m_count->setToolTip(QDir::toNativeSeparators(m_library->directory()));
    auto* import = Holo::pillButton(Icon::Plus, tr("Import"));
    connect(import, &ClayButton::clicked, this, &PhotoPage::importPhotos);

    m_grid = new QListWidget;
    m_grid->setObjectName(QStringLiteral("gallery"));
    m_grid->setViewMode(QListView::IconMode);
    m_grid->setIconSize(ThumbSize);
    m_grid->setItemDelegate(new ThumbDelegate(ThumbSize, 12, Theme::radiusControl, 1.0, m_grid));
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

    auto* layout = new QVBoxLayout(gallery);
    layout->setContentsMargins(Theme::gap, Theme::gap - 6, Theme::gap - 6, Theme::gap - 6);
    layout->setSpacing(4);
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
    m_filmstrip->setItemDelegate(new ThumbDelegate(FilmSize, 12, 8, 0.55, m_filmstrip));
    m_filmstrip->setMouseTracking(true);
    m_filmstrip->setFixedHeight(FilmSize.height() + 24 + 12);
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

    connect(m_prev, &ClayButton::clicked, this, [this] { openPhoto(m_current - 1); });
    connect(m_next, &ClayButton::clicked, this, [this] { openPhoto(m_current + 1); });
    connect(close, &ClayButton::clicked, this, &PhotoPage::closeViewer);
    connect(remove, &ClayButton::clicked, this, &PhotoPage::deleteCurrent);

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
        item->setToolTip(tip);
        auto* film = new QListWidgetItem(QIcon(thumb), QString(), m_filmstrip);
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
    if (!isViewerOpen()) {
        // the viewer floats over the gallery: keep a blurred copy of it as the glass underlay
        if (QWidget* host = Glass::Backdrop::instance().hostFor(m_gallery); host && m_gallery->isVisible()) {
            constexpr int Downscale = 4;
            const QImage snapshot = m_gallery->grab().toImage().scaled(m_gallery->size() / Downscale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            m_viewer->setUnderlay(Glass::blur(snapshot, 24.0 / Downscale), QRect(m_gallery->mapTo(host, QPoint(0, 0)), m_gallery->size()));
        }
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

} // namespace glass
