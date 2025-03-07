#include "PdaWindow.hpp"

#include "Theme.hpp"
#include "pages/BlueprintsPage.hpp"
#include "pages/EncyclopediaPage.hpp"
#include "pages/InventoryPage.hpp"
#include "pages/LogPage.hpp"
#include "pages/PhotoPage.hpp"
#include "pages/PingPage.hpp"
#include "widgets/Glass.hpp"
#include "widgets/PageTransition.hpp"
#include "widgets/TabBar.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QShortcut>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace {
constexpr qreal Margin = 22; // window edge to screen edge
constexpr qreal Radius = 38;
}

PdaWindow::PdaWindow(PhotoLibrary* photos, QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(tr("PDA"));
    setMinimumSize(760, 460);

    m_tabs = new TabBar;
    m_stack = new QStackedWidget;
    QSizePolicy keepSpace = m_stack->sizePolicy();
    keepSpace.setRetainSizeWhenHidden(true); // hidden while a transition plays in its place
    m_stack->setSizePolicy(keepSpace);
    m_transition = new PageTransition(this);
    connect(m_transition, &PageTransition::finished, m_stack, &QWidget::show);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(int(Margin + 34), int(Margin + 8), int(Margin + 34), int(Margin + 22));
    layout->setSpacing(8);
    layout->addWidget(m_tabs);
    layout->addWidget(m_stack, 1);

    addPage(Icon::Person, tr("Inventory"), new InventoryPage);
    addPage(Icon::Wrench, tr("Blueprints"), new BlueprintsPage);
    addPage(Icon::Pin, tr("Ping Manager"), new PingPage);
    addPage(Icon::Image, tr("Photo Manager"), new PhotoPage(photos));
    addPage(Icon::Doc, tr("Log"), new LogPage);
    addPage(Icon::Book, tr("Encyclopedia"), new EncyclopediaPage);
    m_tabs->setBadge(1, 1);
    m_tabs->setBadge(4, LogPage::unreadCount());

    connect(m_tabs, &TabBar::currentChanged, this, &PdaWindow::showPage);

    // the canvas under the glass moves slowly; every glass layer re-samples it each frame
    Glass::Backdrop::instance().setHost(this);
    connect(&m_frames, &QTimer::timeout, this, [this] {
        Glass::Backdrop::instance().render();
        update();
    });
    if (!Glass::settings().reducedMotion) {
        m_frames.start(33);
    }

    // keyboard: 1..6 jump to a page, Q / E step through them
    for (int i = 0; i < m_tabs->count(); ++i) {
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

void PdaWindow::addPage(Icon icon, const QString& name, QWidget* page)
{
    m_tabs->addTab(icon, name);
    m_stack->addWidget(page);
}

void PdaWindow::showPage(int index)
{
    const int from = m_stack->currentIndex();
    if (index == from) {
        return;
    }
    if (!m_animate || !isVisible() || from < 0) {
        m_stack->setCurrentIndex(index);
        return;
    }
    m_transition->finish(); // a click during a transition starts the next one from the settled page
    const QPixmap before = m_stack->currentWidget()->grab();
    m_stack->setCurrentIndex(index);
    const QPixmap after = m_stack->currentWidget()->grab();
    m_transition->setGeometry(m_stack->geometry());
    m_stack->hide();
    m_transition->run(before, after, index > from ? 1 : -1);
}

int PdaWindow::pageCount() const { return m_stack->count(); }

int PdaWindow::currentPage() const { return m_stack->currentIndex(); }

void PdaWindow::setPage(int index) { m_tabs->setCurrentIndex(index); }

int PdaWindow::pageIndex(QWidget* page) const { return m_stack->indexOf(page); }

QRectF PdaWindow::screenRect() const
{
    return QRectF(rect()).adjusted(Margin, Margin, -Margin, -Margin);
}

void PdaWindow::resizeEvent(QResizeEvent* event)
{
    Glass::Backdrop::instance().render();
    QWidget::resizeEvent(event);
}

void PdaWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.fillRect(rect(), Theme::outside);

    const QRectF screen = screenRect();
    // dark-mode radiance marks the edge of the screen instead of a frame
    Glass::paintGlow(p, screen, Radius, Theme::glowAccent, 0.7);

    QPainterPath shape;
    shape.addRoundedRect(screen, Radius, Radius);
    p.save();
    p.setClipPath(shape);
    p.drawImage(QRectF(rect()), Glass::Backdrop::instance().canvas());
    p.restore();

    QLinearGradient light(screen.topLeft(), screen.bottomRight());
    light.setColorAt(0, Theme::glassHighlight);
    light.setColorAt(0.5, Theme::glassBorder);
    light.setColorAt(1, Theme::glassBorder);
    p.strokePath(shape, QPen(QBrush(light), 1));
}
