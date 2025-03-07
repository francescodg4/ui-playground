#include "PdaWindow.hpp"

#include "PhotoLibrary.hpp"
#include "Theme.hpp"
#include "pages/BlueprintsPage.hpp"
#include "pages/EncyclopediaPage.hpp"
#include "pages/InventoryPage.hpp"
#include "pages/LogPage.hpp"
#include "pages/PhotoPage.hpp"
#include "pages/PingPage.hpp"
#include "widgets/Chrome.hpp"
#include "widgets/Metal.hpp"
#include "widgets/Telemetry.hpp"

#include <QPainter>
#include <QShortcut>
#include <QStackedWidget>
#include <QVBoxLayout>

PdaWindow::PdaWindow(PhotoLibrary* photos, QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(tr("PDA"));
    setWindowFlag(Qt::FramelessWindowHint); // the skin draws its own title bar
    setMinimumSize(820, 560);

    m_menu = new MenuStrip;
    m_telemetry = new Telemetry;
    m_stack = new QStackedWidget;

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 10);
    layout->setSpacing(4);
    layout->addWidget(new TitleBar(tr("PDA")));
    layout->addWidget(m_menu);
    layout->addWidget(m_telemetry);
    layout->addSpacing(4);
    layout->addWidget(m_stack, 1);

    addPage(tr("&Inventory"), new InventoryPage);
    addPage(tr("&Blueprints"), new BlueprintsPage);
    addPage(tr("P&ings"), new PingPage);
    addPage(tr("&Photos"), new PhotoPage(photos));
    addPage(tr("&Log"), new LogPage);
    addPage(tr("&Encyclopedia"), new EncyclopediaPage);

    m_telemetry->setCounter(tr("PHOTOS"), int(photos->photos().size()));
    m_telemetry->setCounter(tr("PINGS"), 3);
    m_telemetry->setCounter(tr("LOG"), LogPage::unreadCount());
    connect(photos, &PhotoLibrary::changed, this, [this, photos] { m_telemetry->setCounter(tr("PHOTOS"), int(photos->photos().size())); });

    connect(m_menu, &MenuStrip::currentChanged, this, &PdaWindow::showPage);
    showPage(0);

    // Alt + underlined letter opens a menu entry; 1..6 and Q / E also work
    for (int i = 0; i < m_menu->count(); ++i) {
        auto* alt = new QShortcut(QKeySequence(QStringLiteral("Alt+") + m_menu->mnemonic(i)), this);
        connect(alt, &QShortcut::activated, this, [this, i] { setPage(i); });
        auto* digit = new QShortcut(QKeySequence(Qt::Key_1 + i), this);
        connect(digit, &QShortcut::activated, this, [this, i] { setPage(i); });
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

void PdaWindow::addPage(const QString& menuText, QWidget* page)
{
    m_menu->addItem(menuText);
    m_stack->addWidget(page);
}

void PdaWindow::showPage(int index)
{
    m_stack->setCurrentIndex(index);
    m_telemetry->setPage(m_menu->itemText(index), m_stack->widget(index)->statusTip());
}

int PdaWindow::pageCount() const { return m_stack->count(); }

int PdaWindow::currentPage() const { return m_stack->currentIndex(); }

void PdaWindow::setPage(int index) { m_menu->setCurrentIndex(index); }

int PdaWindow::pageIndex(QWidget* page) const { return m_stack->indexOf(page); }

void PdaWindow::setAnimationsEnabled(bool enabled) { m_telemetry->setAnimated(enabled); }

void PdaWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    Metal::frame(p, QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), Theme::radiusPanel);
    // etched groove around the page module
    Metal::bevel(p, QRectF(m_stack->geometry()).adjusted(-3, -3, 3, 3), Theme::radiusPanel, false);
}
