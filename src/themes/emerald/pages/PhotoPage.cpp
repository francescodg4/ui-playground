#include "PhotoPage.hpp"

#include "PhotoLibrary.hpp"
#include "themes/emerald/Theme.hpp"
#include "themes/emerald/widgets/Pixel.hpp"
#include "themes/emerald/widgets/PixelWidgets.hpp"

#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QShortcut>
#include <QStackedWidget>
#include <QStyledItemDelegate>

namespace emerald {

using Pixel::Scale;

namespace {
const QSize ThumbPixels(48, 27); // low-resolution thumbnail, in pixels
constexpr int ThumbRole = Qt::UserRole;

/// Thumbnails as box slots: highlighted tile for the chosen photo, glove over the hovered one.
class SlotDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        return QSize(56 * Scale, 46 * Scale);
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        Pixel::Canvas canvas(*painter, option.rect);
        QPainter& p = canvas.p();
        const QRect frame(3, 10, ThumbPixels.width() + 4, ThumbPixels.height() + 4);
        if (option.state & QStyle::State_Selected) {
            Pixel::highlightTile(p, frame.adjusted(-2, -2, 2, 2));
        }
        Pixel::chamfer(p, frame, Theme::borderDark, Theme::borderDark, 1);
        p.drawImage(frame.left() + 2, frame.top() + 2, index.data(ThumbRole).value<QImage>());
        if (option.state & QStyle::State_MouseOver) {
            Pixel::hand(p, QPoint(frame.center().x(), frame.top() + 2), Pixel::Direction::Down);
        }
    }
};

} // namespace

/// Metallic data window whose CRT screen shows a photo at low resolution, with scanlines.
class DataScreen : public QWidget {
public:
    explicit DataScreen(const QString& title)
        : m_title(title)
    {
        setMinimumSize(120 * Scale, 90 * Scale);
    }

    void setPhoto(const QImage& image, const QString& title)
    {
        m_image = image;
        m_title = title;
        m_cache = {};
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        Pixel::Canvas canvas(this);
        QPainter& p = canvas.p();
        const QRect screen = Pixel::dataWindow(p, canvas.rect(), m_title);
        if (!m_image.isNull()) {
            const QSize fit = m_image.size().scaled(screen.size(), Qt::KeepAspectRatio);
            if (m_cache.size() != fit) {
                m_cache = Pixel::pixelate(m_image, fit);
            }
            p.drawImage(screen.left() + (screen.width() - fit.width()) / 2, screen.top() + (screen.height() - fit.height()) / 2, m_cache);
        } else {
            p.setFont(Pixel::font(true));
            Pixel::text(p, screen, Qt::AlignCenter, tr("NO DATA"), Theme::lightText, Theme::crtDark.darker(150));
        }
        Pixel::scanlines(p, screen);
    }

private:
    QString m_title;
    QImage m_image;
    QImage m_cache;
};

PhotoPage::PhotoPage(PhotoLibrary* library, QWidget* parent)
    : QWidget(parent)
    , m_library(library)
{
    m_views = new QStackedWidget;
    m_views->addWidget(buildGallery());
    m_views->addWidget(buildViewer());

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4 * Scale, 2 * Scale, 4 * Scale, 2 * Scale);
    layout->addWidget(m_views);

    connect(m_library, &PhotoLibrary::changed, this, &PhotoPage::rebuild);
    rebuild();
}

QWidget* PhotoPage::buildGallery()
{
    auto* gallery = new QWidget;

    m_preview = new DataScreen(tr("PHOTO DATA"));
    m_preview->setFixedSize(168 * Scale, 116 * Scale);

    auto* infoBox = new PaperBox;
    m_info = new PixelLabel;
    m_info->setWordWrap(true);
    auto* infoLayout = new QVBoxLayout(infoBox);
    infoLayout->setContentsMargins(7 * Scale, 5 * Scale, 7 * Scale, 5 * Scale);
    infoLayout->addWidget(m_info);
    infoBox->setFixedWidth(168 * Scale);

    m_view = new PixelKey(tr("OK VIEW"), Theme::keyYellow);
    auto* import = new PixelKey(tr("IMPORT"), Theme::kbdNumGreen);
    connect(m_view, &PixelKey::clicked, this, [this] { openPhoto(m_selected); });
    connect(import, &PixelKey::clicked, this, &PhotoPage::importPhotos);

    auto* keys = new QHBoxLayout;
    keys->addWidget(m_view);
    keys->addWidget(import);
    keys->addStretch();

    auto* left = new QVBoxLayout;
    left->setSpacing(4 * Scale);
    left->addWidget(m_preview);
    left->addWidget(infoBox);
    left->addLayout(keys);
    left->addStretch();

    m_count = new PixelLabel(QString(), PixelLabel::Tone::Light, true);
    m_count->setToolTip(QDir::toNativeSeparators(m_library->directory()));

    m_grid = new QListWidget;
    m_grid->setViewMode(QListView::IconMode);
    m_grid->setMovement(QListView::Static);
    m_grid->setResizeMode(QListView::Adjust);
    m_grid->setUniformItemSizes(true);
    m_grid->setSelectionMode(QAbstractItemView::SingleSelection);
    m_grid->setItemDelegate(new SlotDelegate(m_grid));
    m_grid->setFocusPolicy(Qt::NoFocus);
    m_grid->setMouseTracking(true);
    m_grid->viewport()->setAutoFillBackground(false);
    m_grid->setCursor(Qt::PointingHandCursor);
    connect(m_grid, &QListWidget::currentRowChanged, this, &PhotoPage::select);
    connect(m_grid, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) { openPhoto(m_grid->row(item)); });

    auto* right = new QVBoxLayout;
    right->addWidget(m_count);
    right->addWidget(m_grid, 1);

    auto* layout = new QHBoxLayout(gallery);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8 * Scale);
    layout->addLayout(left);
    layout->addLayout(right, 1);
    return gallery;
}

QWidget* PhotoPage::buildViewer()
{
    m_viewer = new QWidget;
    m_viewer->setFocusPolicy(Qt::StrongFocus);

    m_screen = new DataScreen(tr("PHOTO DATA"));
    m_prev = new PixelKey(tr("< PREV"), Theme::kbdUpperBlue);
    m_next = new PixelKey(tr("NEXT >"), Theme::kbdUpperBlue);
    auto* remove = new PixelKey(tr("DELETE"), Theme::kbdLowerOrange);
    auto* back = new PixelKey(tr("B BACK"), Theme::keyGray);
    m_counter = new PixelLabel(QString(), PixelLabel::Tone::Light, true);

    auto* keys = new QHBoxLayout;
    keys->setSpacing(4 * Scale);
    keys->addWidget(m_prev);
    keys->addWidget(m_next);
    keys->addSpacing(6 * Scale);
    keys->addWidget(m_counter);
    keys->addStretch();
    keys->addWidget(remove);
    keys->addWidget(back);

    auto* layout = new QVBoxLayout(m_viewer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4 * Scale);
    layout->addWidget(m_screen, 1);
    layout->addLayout(keys);

    connect(m_prev, &PixelKey::clicked, this, [this] { openPhoto(m_current - 1); });
    connect(m_next, &PixelKey::clicked, this, [this] { openPhoto(m_current + 1); });
    connect(back, &PixelKey::clicked, this, &PhotoPage::closeViewer);
    connect(remove, &PixelKey::clicked, this, &PhotoPage::deleteCurrent);

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

QImage PhotoPage::thumbnail(int index)
{
    const Photo& photo = m_library->photos().at(index);
    const QString key = photo.path + QString::number(photo.acquired.toMSecsSinceEpoch());
    auto it = m_thumbnails.find(key);
    if (it == m_thumbnails.end()) {
        const QImage full(photo.path);
        const QImage cropped = full.isNull() ? full : full.scaled(ThumbPixels * 8, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const QImage center = cropped.isNull() ? QImage(ThumbPixels, QImage::Format_RGB32)
                                               : cropped.copy(QRect(QPoint((cropped.width() - ThumbPixels.width() * 8) / 2, (cropped.height() - ThumbPixels.height() * 8) / 2), ThumbPixels * 8));
        it = m_thumbnails.insert(key, Pixel::pixelate(center, ThumbPixels));
    }
    return *it;
}

void PhotoPage::rebuild()
{
    const QList<Photo>& photos = m_library->photos();
    m_count->setText(photos.size() == 1 ? tr("1 PHOTO") : tr("%1 PHOTOS").arg(photos.size()));

    m_grid->blockSignals(true);
    m_grid->clear();
    for (int i = 0; i < photos.size(); ++i) {
        auto* item = new QListWidgetItem(m_grid);
        item->setData(ThumbRole, thumbnail(i));
        item->setToolTip(photos[i].name);
    }
    m_grid->blockSignals(false);

    const int keep = photos.isEmpty() ? -1 : std::clamp(m_selected, 0, int(photos.size()) - 1);
    m_selected = -2; // force a refresh
    select(keep);

    if (isViewerOpen()) {
        if (photos.isEmpty()) {
            closeViewer();
        } else {
            const int index = std::min(m_current, int(photos.size()) - 1);
            m_current = -1;
            openPhoto(index);
        }
    }
}

void PhotoPage::select(int index)
{
    const QList<Photo>& photos = m_library->photos();
    if (index == m_selected) {
        return;
    }
    m_selected = index;
    if (index < 0 || index >= photos.size()) {
        m_preview->setPhoto({}, tr("PHOTO DATA"));
        m_info->setText(tr("No photos acquired yet."));
        m_view->setEnabled(false);
        return;
    }
    const Photo& photo = photos[index];
    m_preview->setPhoto(QImage(photo.path), tr("PHOTO DATA"));
    m_info->setText(QStringLiteral("%1\n%2\nNo. %3/%4").arg(photo.name, photo.acquired.toString(QStringLiteral("yyyy-MM-dd HH:mm"))).arg(index + 1).arg(photos.size()));
    m_view->setEnabled(true);
    const QSignalBlocker block(m_grid);
    m_grid->setCurrentRow(index);
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
        m_screen->setPhoto(QImage(photo.path), photo.name.toUpper());
        m_counter->setText(QStringLiteral("%1/%2").arg(index + 1).arg(photos.size()));
        m_prev->setEnabled(index > 0);
        m_next->setEnabled(index < photos.size() - 1);
        select(index);
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

} // namespace emerald
