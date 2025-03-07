#include "themes/emerald/PdaWindow.hpp"

#include "themes/emerald/Theme.hpp"
#include "themes/emerald/pages/BlueprintsPage.hpp"
#include "themes/emerald/pages/EncyclopediaPage.hpp"
#include "themes/emerald/pages/InventoryPage.hpp"
#include "themes/emerald/pages/LogPage.hpp"
#include "themes/emerald/pages/PhotoPage.hpp"
#include "themes/emerald/pages/PingPage.hpp"
#include "themes/emerald/widgets/BoxHeader.hpp"
#include "themes/emerald/widgets/PixelWidgets.hpp"

#include <QShortcut>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace emerald {

using Pixel::Scale;

namespace {
constexpr int Margin = 8 * Scale; // window edge to the wallpaper container
constexpr int Frame = 6 * Scale; // container frame to page content
}

PdaWindow::PdaWindow(PhotoLibrary* photos, QWidget* parent)
    : PdaShell(parent)
{
    setWindowTitle(tr("PDA"));
    setMinimumSize(760, 480);

    m_header = new BoxHeader;
    m_stack = new QStackedWidget;

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(Margin + Frame, 0, Margin + Frame, Margin + Frame);
    layout->setSpacing(Frame + 2 * Scale);
    auto* select = new PixelKey(tr("SELECT"), Theme::keyGray);
    select->setToolTip(tr("Select theme (Ctrl+T)"));
    connect(select, &PixelKey::clicked, this, &PdaShell::selectThemeRequested);
    auto* top = new QHBoxLayout;
    top->addSpacing(select->sizeHint().width());
    top->addWidget(m_header, 1);
    top->addWidget(select, 0, Qt::AlignBottom);
    layout->addLayout(top);
    layout->addWidget(m_stack, 1);

    addPage(Icon::Person, tr("Inventory"), Pixel::Wallpaper::Forest, new InventoryPage);
    addPage(Icon::Wrench, tr("Blueprints"), Pixel::Wallpaper::Metallic, new BlueprintsPage);
    addPage(Icon::Pin, tr("Ping Manager"), Pixel::Wallpaper::Sky, new PingPage);
    addPage(Icon::Image, tr("Photo Manager"), Pixel::Wallpaper::Seafloor, new PhotoPage(photos));
    addPage(Icon::Doc, tr("Log"), Pixel::Wallpaper::Simple, new LogPage);
    addPage(Icon::Book, tr("Encyclopedia"), Pixel::Wallpaper::River, new EncyclopediaPage);
    m_header->setBadge(1, 1);
    m_header->setBadge(4, LogPage::unreadCount());

    connect(m_header, &BoxHeader::currentChanged, this, [this](int index) {
        m_stack->setCurrentIndex(index);
        update(); // the wallpaper follows the page
    });

    // controller-style navigation: 1..6 jump, Q / E are the L / R shoulder buttons
    for (int i = 0; i < m_header->count(); ++i) {
        auto* shortcut = new QShortcut(QKeySequence(Qt::Key_1 + i), this);
        connect(shortcut, &QShortcut::activated, this, [this, i] { setPage(i); });
    }
    connect(new QShortcut(QKeySequence(Qt::Key_Q), this), &QShortcut::activated, this, [this] {
        setPage((currentPage() + pageCount() - 1) % pageCount());
    });
    connect(new QShortcut(QKeySequence(Qt::Key_E), this), &QShortcut::activated, this, [this] {
        setPage((currentPage() + 1) % pageCount());
    });
    connect(new QShortcut(QKeySequence(Qt::Key_F11), this), &QShortcut::activated, this, [this] {
        setWindowState(windowState() ^ Qt::WindowFullScreen);
    });
}

void PdaWindow::addPage(Icon icon, const QString& name, Pixel::Wallpaper wallpaper, QWidget* page)
{
    m_header->addTab(icon, name, wallpaper);
    m_stack->addWidget(page);
}

void PdaWindow::showPhotoViewer()
{
    if (auto* page = findChild<PhotoPage*>()) {
        setPage(pageIndex(page));
        page->openPhoto(0);
    }
}

int PdaWindow::pageCount() const { return m_stack->count(); }

int PdaWindow::currentPage() const { return m_stack->currentIndex(); }

void PdaWindow::setPage(int index) { m_header->setCurrentIndex(index); }

int PdaWindow::pageIndex(QWidget* page) const { return m_stack->indexOf(page); }

void PdaWindow::paintEvent(QPaintEvent*)
{
    Pixel::Canvas canvas(this);
    QPainter& p = canvas.p();

    // emerald backdrop with a low-contrast 16×16 diamond pattern
    p.fillRect(canvas.rect(), Theme::primaryGreen);
    const QColor diamond = Theme::primaryGreen.darker(112);
    for (int y = 0; y < canvas.rect().height(); y += 16) {
        for (int x = 0; x < canvas.rect().width(); x += 16) {
            for (int i = 0; i < 4; ++i) {
                p.fillRect(x + 8 - i, y + 4 + i, 2 * i + 1, 1, diamond);
                p.fillRect(x + 8 - i, y + 11 - i, 2 * i + 1, 1, diamond);
            }
        }
    }

    // wallpaper container of the current page
    const QRect page = m_stack->geometry();
    const QRect box(page.left() / Scale - Frame / Scale, page.top() / Scale - Frame / Scale,
        page.width() / Scale + 2 * Frame / Scale, page.height() / Scale + 2 * Frame / Scale);
    Pixel::chamfer(p, box.translated(2, 2), Theme::darkGreen, Theme::darkGreen, 3); // bi-colour drop shadow
    Pixel::wallpaper(p, box, m_header->wallpaper(m_header->currentIndex()));
}

} // namespace emerald
