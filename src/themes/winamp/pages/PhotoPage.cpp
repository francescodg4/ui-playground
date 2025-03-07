#include "PhotoPage.hpp"

#include "PhotoLibrary.hpp"
#include "themes/winamp/Theme.hpp"
#include "themes/winamp/widgets/Metal.hpp"
#include "themes/winamp/widgets/MetalWidgets.hpp"
#include "themes/winamp/widgets/Telemetry.hpp"

#include <QDir>
#include <QFileDialog>
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

namespace winamp {

namespace {
const QSize ThumbSize(144, 81);

QPixmap cover(const QImage& image, const QSize& size)
{
    if (image.isNull()) {
        QPixmap empty(size);
        empty.fill(Theme::lcdBg);
        return empty;
    }
    const QImage scaled = image.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    return QPixmap::fromImage(scaled.copy(QRect(QPoint((scaled.width() - size.width()) / 2, (scaled.height() - size.height()) / 2), size)));
}

/// Thumbnails framed in a bevel on the LCD; hover lights the row colour, selection glows.
class ThumbDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override { return ThumbSize + QSize(16, 16); }

    void paint(QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        const QRectF cell = QRectF(option.rect).adjusted(3, 3, -3, -3);
        if (option.state & QStyle::State_MouseOver) {
            QPainterPath bg;
            bg.addRoundedRect(cell, Theme::radiusPanel, Theme::radiusPanel);
            p->fillPath(bg, Theme::lcdRow);
        }
        const QRectF frame = QRectF(option.rect).adjusted(8, 8, -8, -8);
        p->drawPixmap(frame.toRect(), index.data(Qt::DecorationRole).value<QIcon>().pixmap(ThumbSize));
        Metal::bevel(*p, frame.adjusted(-1, -1, 1, 1), 2, false);
        if (option.state & QStyle::State_Selected) {
            QColor halo = Theme::lcdGlow;
            halo.setAlpha(90);
            p->setPen(QPen(halo, 5));
            p->drawRoundedRect(frame.adjusted(-3, -3, 3, 3), 4, 4);
            p->setPen(QPen(Theme::lcdGlow, 1.5));
            p->drawRoundedRect(frame.adjusted(-3, -3, 3, 3), 4, 4);
        }
        p->restore();
    }
};

} // namespace

/// Bottom docking tabs switching the page's modules.
class DockTabs : public QWidget {
public:
    std::function<void(int)> onSelect;

    explicit DockTabs(const QStringList& tabs)
        : m_tabs(tabs)
    {
        setFixedHeight(26);
        setFont(Metal::uiFont(9, true));
        setCursor(Qt::PointingHandCursor);
    }

    void setCurrent(int index)
    {
        m_current = index;
        update();
    }

protected:
    QRectF tabRect(int i) const { return QRectF(12 + i * 132, 0, 140, height() - 2); }

    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        for (int i = int(m_tabs.size()) - 1; i >= 0; --i) { // current tab drawn last, on top
            if (i != m_current) {
                paintTab(p, i);
            }
        }
        paintTab(p, m_current);
    }

    void paintTab(QPainter& p, int i)
    {
        const QRectF r = tabRect(i);
        QPainterPath tab;
        tab.moveTo(r.left(), r.top());
        tab.lineTo(r.right(), r.top());
        tab.lineTo(r.right() - 14, r.bottom() - 4);
        tab.quadTo(r.right() - 16, r.bottom(), r.right() - 20, r.bottom());
        tab.lineTo(r.left() + 20, r.bottom());
        tab.quadTo(r.left() + 16, r.bottom(), r.left() + 14, r.bottom() - 4);
        tab.closeSubpath();
        QLinearGradient g(r.topLeft(), r.bottomLeft());
        g.setColorAt(0, i == m_current ? QColor(0xf4, 0xf6, 0xfa) : QColor(0xc3, 0xca, 0xd6));
        g.setColorAt(1, i == m_current ? QColor(0xd8, 0xde, 0xe8) : QColor(0xa0, 0xaa, 0xb8));
        p.fillPath(tab, g);
        p.strokePath(tab, QPen(Theme::outline, 1));
        p.setPen(i == m_current ? Theme::text : Theme::textDim);
        p.drawText(r, Qt::AlignCenter, m_tabs[i]);
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        for (int i = 0; i < m_tabs.size(); ++i) {
            if (tabRect(i).adjusted(12, 0, -12, 0).contains(e->position()) && onSelect) {
                onSelect(i);
            }
        }
    }

private:
    QStringList m_tabs;
    int m_current = 0;
};

/// The open photo, fitted inside the LCD.
class PhotoView : public QWidget {
public:
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
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        const QSizeF size = QSizeF(m_pixmap.size()).scaled(QSizeF(this->size()) - QSizeF(4, 4), Qt::KeepAspectRatio);
        const QRectF target(QPointF((width() - size.width()) / 2, (height() - size.height()) / 2), size);
        p.drawPixmap(target, m_pixmap, QRectF(m_pixmap.rect()));
        Metal::bevel(p, target.adjusted(-1, -1, 1, 1), 2, false);
    }

private:
    QPixmap m_pixmap;
};

PhotoPage::PhotoPage(PhotoLibrary* library, QWidget* parent)
    : QWidget(parent)
    , m_library(library)
{
    m_views = new QStackedWidget;
    m_views->addWidget(buildGallery());
    m_views->addWidget(buildViewer());

    m_tabs = new DockTabs({ tr("GALLERY"), tr("VIEWER") });
    m_tabs->onSelect = [this](int i) { i == 0 ? closeViewer() : openPhoto(std::max(0, m_current)); };

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 0);
    layout->setSpacing(0);
    layout->addWidget(m_views, 1);
    layout->addWidget(m_tabs);

    m_slideshow.setInterval(2500);
    connect(&m_slideshow, &QTimer::timeout, this, [this] {
        if (m_current + 1 < m_library->photos().size()) {
            openPhoto(m_current + 1);
        } else {
            setSlideshow(false); // end of the list
        }
    });

    connect(m_library, &PhotoLibrary::changed, this, &PhotoPage::rebuild);
    rebuild();
}

QWidget* PhotoPage::buildGallery()
{
    auto* panel = new LcdPanel;
    m_count = MetalUi::lcdLabel(QString());
    m_count->setToolTip(QDir::toNativeSeparators(m_library->directory()));
    auto* import = new CapsuleButton(tr("IMPORT"), Metal::Glyph::Eject);
    connect(import, &CapsuleButton::clicked, this, &PhotoPage::importPhotos);

    m_grid = new QListWidget;
    m_grid->setViewMode(QListView::IconMode);
    m_grid->setIconSize(ThumbSize);
    m_grid->setMovement(QListView::Static);
    m_grid->setResizeMode(QListView::Adjust);
    m_grid->setUniformItemSizes(true);
    m_grid->setItemDelegate(new ThumbDelegate(m_grid));
    m_grid->setFocusPolicy(Qt::NoFocus);
    m_grid->setMouseTracking(true);
    m_grid->viewport()->setAutoFillBackground(false);
    m_grid->setCursor(Qt::PointingHandCursor);
    connect(m_grid, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        const Photo& photo = m_library->photos().at(m_grid->row(item));
        Telemetry::report(tr("%1 - %2").arg(photo.name, photo.acquired.toString(QStringLiteral("yyyy-MM-dd HH:mm"))));
    });
    connect(m_grid, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) { openPhoto(m_grid->row(item)); });

    auto* bar = new QHBoxLayout;
    bar->addWidget(m_count);
    bar->addStretch();
    bar->addWidget(import);

    auto* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(12, 8, 10, 8);
    layout->addLayout(bar);
    layout->addWidget(m_grid, 1);
    return panel;
}

QWidget* PhotoPage::buildViewer()
{
    m_viewer = new QWidget;
    m_viewer->setFocusPolicy(Qt::StrongFocus);

    auto* screen = new LcdPanel;
    m_view = new PhotoView;
    auto* screenLayout = new QVBoxLayout(screen);
    screenLayout->setContentsMargins(10, 10, 10, 10);
    screenLayout->addWidget(m_view);

    m_position = new PositionSlider;
    connect(m_position, &PositionSlider::valueChanged, this, &PhotoPage::openPhoto);
    auto* eject = new CapsuleButton(QString(), Metal::Glyph::Eject);
    eject->setToolTip(tr("Import photos"));
    auto* playlist = new CapsuleButton(tr("PL"));
    playlist->setToolTip(tr("Back to the gallery (Esc)"));
    auto* remove = new CapsuleButton(tr("DEL"));
    remove->setToolTip(tr("Delete photo (Del)"));
    connect(eject, &CapsuleButton::clicked, this, &PhotoPage::importPhotos);
    connect(playlist, &CapsuleButton::clicked, this, &PhotoPage::closeViewer);
    connect(remove, &CapsuleButton::clicked, this, &PhotoPage::deleteCurrent);

    auto* seek = new QHBoxLayout;
    seek->setSpacing(6);
    seek->addWidget(m_position, 1);
    seek->addWidget(eject);
    seek->addWidget(playlist);
    seek->addWidget(remove);

    m_prev = new RoundButton(Metal::Glyph::Rewind, 40);
    m_play = new RoundButton(Metal::Glyph::Play, 40);
    auto* pause = new RoundButton(Metal::Glyph::Pause, 40);
    auto* stop = new RoundButton(Metal::Glyph::Stop, 40);
    m_next = new RoundButton(Metal::Glyph::Forward, 40);
    m_prev->setToolTip(tr("Previous (Left)"));
    m_play->setToolTip(tr("Slideshow (Space)"));
    m_next->setToolTip(tr("Next (Right)"));
    connect(m_prev, &RoundButton::clicked, this, [this] { openPhoto(m_current - 1); });
    connect(m_next, &RoundButton::clicked, this, [this] { openPhoto(m_current + 1); });
    connect(m_play, &RoundButton::clicked, this, [this] { setSlideshow(true); });
    connect(pause, &RoundButton::clicked, this, [this] { setSlideshow(false); });
    connect(stop, &RoundButton::clicked, this, [this] {
        setSlideshow(false);
        openPhoto(0);
    });

    auto* deck = new QHBoxLayout;
    deck->setSpacing(4);
    for (RoundButton* b : { m_prev, m_play, pause, stop, m_next }) {
        deck->addWidget(b);
    }
    deck->addStretch();

    auto* layout = new QVBoxLayout(m_viewer);
    layout->setContentsMargins(0, 0, 0, 4);
    layout->setSpacing(6);
    layout->addWidget(screen, 1);
    layout->addLayout(seek);
    layout->addLayout(deck);

    const auto shortcut = [this](QKeySequence key, auto slot) {
        auto* s = new QShortcut(key, m_viewer);
        s->setContext(Qt::WidgetWithChildrenShortcut);
        connect(s, &QShortcut::activated, this, slot);
    };
    shortcut(QKeySequence(Qt::Key_Left), [this] { openPhoto(m_current - 1); });
    shortcut(QKeySequence(Qt::Key_Right), [this] { openPhoto(m_current + 1); });
    shortcut(QKeySequence(Qt::Key_Escape), [this] { closeViewer(); });
    shortcut(QKeySequence(Qt::Key_Delete), [this] { deleteCurrent(); });
    shortcut(QKeySequence(Qt::Key_Space), [this] { setSlideshow(!m_slideshow.isActive()); });
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
    const QString count = photos.size() == 1 ? tr("1 PHOTO") : tr("%1 PHOTOS").arg(photos.size());
    m_count->setText(count);
    setStatusTip(count.toLower());

    m_grid->clear();
    for (int i = 0; i < photos.size(); ++i) {
        auto* item = new QListWidgetItem(QIcon(thumbnail(i)), QString(), m_grid);
        item->setToolTip(photos[i].name);
    }
    m_position->setMaximum(std::max(0, int(photos.size()) - 1));

    if (isViewerOpen()) {
        if (photos.isEmpty()) {
            closeViewer();
        } else {
            const int keep = std::min(m_current, int(photos.size()) - 1);
            m_current = -1;
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
        m_position->setValue(index);
        m_prev->setEnabled(index > 0);
        m_next->setEnabled(index < photos.size() - 1);
        m_grid->setCurrentRow(index);
        Telemetry::report(tr("Photo %1/%2 - %3 - %4").arg(index + 1).arg(photos.size()).arg(photo.name, photo.acquired.toString(QStringLiteral("yyyy-MM-dd HH:mm"))));
    }
    m_views->setCurrentWidget(m_viewer);
    m_tabs->setCurrent(1);
    m_viewer->setFocus();
}

void PhotoPage::closeViewer()
{
    setSlideshow(false);
    m_views->setCurrentIndex(0);
    m_tabs->setCurrent(0);
}

bool PhotoPage::isViewerOpen() const
{
    return m_views->currentWidget() == m_viewer;
}

void PhotoPage::setSlideshow(bool running)
{
    running ? m_slideshow.start() : m_slideshow.stop();
    m_play->setActive(running); // blue-accented Play while the slideshow runs
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

} // namespace winamp
