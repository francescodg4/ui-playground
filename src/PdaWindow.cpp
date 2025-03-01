#include "PdaWindow.hpp"

#include "Theme.hpp"
#include "widgets/Holo.hpp"
#include "widgets/TabBar.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QRandomGenerator>
#include <QStackedWidget>
#include <QVBoxLayout>

#include <cmath>

namespace {
constexpr qreal Bezel = 12; // blue plastic rim around the screen
constexpr qreal Margin = 22; // window edge to screen edge
constexpr qreal Radius = 38;
constexpr qreal DotSpacing = 30;

QWidget* placeholder(const QString& title)
{
    auto* page = new QWidget;
    Holo::pageLayout(page, title)->addStretch();
    return page;
}
}

PdaWindow::PdaWindow(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(tr("PDA"));
    setMinimumSize(760, 460);

    m_tabs = new TabBar;
    m_stack = new QStackedWidget;

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(int(Margin + 34), int(Margin + 8), int(Margin + 34), int(Margin + 22));
    layout->setSpacing(8);
    layout->addWidget(m_tabs);
    layout->addWidget(m_stack, 1);

    addPage(Icon::Person, tr("Inventory"), placeholder(tr("Inventory")));
    addPage(Icon::Wrench, tr("Blueprints"), placeholder(tr("Blueprints")));
    addPage(Icon::Pin, tr("Ping Manager"), placeholder(tr("Ping Manager")));
    addPage(Icon::Image, tr("Photo Manager"), placeholder(tr("Photo Manager")));
    addPage(Icon::Doc, tr("Log"), placeholder(tr("Log")));
    addPage(Icon::Book, tr("Encyclopedia"), placeholder(tr("Encyclopedia")));
    m_tabs->setBadge(1, 1);
    m_tabs->setBadge(4, 4);

    connect(m_tabs, &TabBar::currentChanged, m_stack, &QStackedWidget::setCurrentIndex);

    // drifting specks in the water
    auto* rng = QRandomGenerator::global();
    for (int i = 0; i < 80; ++i) {
        m_specks.append({ rng->generateDouble(), rng->generateDouble(), 0.5 + 1.3 * rng->generateDouble(),
            0.004 + 0.012 * rng->generateDouble(), 0.2 + 0.5 * rng->generateDouble(), 6.28 * rng->generateDouble() });
    }
    m_clock.start();
    connect(&m_animation, &QTimer::timeout, this, [this] { update(screenRect().toAlignedRect()); });
    m_animation.start(40);
}

void PdaWindow::addPage(Icon icon, const QString& name, QWidget* page)
{
    m_tabs->addTab(icon, name);
    m_stack->addWidget(page);
}

int PdaWindow::pageCount() const { return m_stack->count(); }

int PdaWindow::currentPage() const { return m_stack->currentIndex(); }

void PdaWindow::setPage(int index) { m_tabs->setCurrentIndex(index); }

QRectF PdaWindow::screenRect() const
{
    return QRectF(rect()).adjusted(Margin, Margin, -Margin, -Margin);
}

void PdaWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // underwater backdrop
    QLinearGradient water(0, 0, 0, height());
    water.setColorAt(0, Theme::backdropTop);
    water.setColorAt(1, Theme::backdropBottom);
    p.fillRect(rect(), water);

    const QRectF screen = screenRect();

    // glow and blue rim
    for (int i = 5; i >= 1; --i) {
        QPainterPath halo;
        halo.addRoundedRect(screen.adjusted(-Bezel - i * 3, -Bezel - i * 3, Bezel + i * 3, Bezel + i * 3), Radius + Bezel + i * 3, Radius + Bezel + i * 3);
        p.fillPath(halo, QColor(70, 180, 255, 18));
    }
    QPainterPath rim;
    rim.addRoundedRect(screen.adjusted(-Bezel, -Bezel, Bezel, Bezel), Radius + Bezel, Radius + Bezel);
    QLinearGradient rimFill(screen.topLeft(), screen.bottomLeft());
    rimFill.setColorAt(0, Theme::bezelTop);
    rimFill.setColorAt(1, Theme::bezelBottom);
    p.fillPath(rim, rimFill);

    paintScreen(p, screen);
}

void PdaWindow::paintScreen(QPainter& p, const QRectF& screen)
{
    const qreal t = m_clock.elapsed() / 1000.0;
    QPainterPath shape;
    shape.addRoundedRect(screen, Radius, Radius);

    QLinearGradient glass(screen.topLeft(), screen.bottomLeft());
    glass.setColorAt(0, Theme::screenTop);
    glass.setColorAt(1, Theme::screenBottom);
    p.fillPath(shape, glass);

    p.save();
    p.setClipPath(shape);

    // light rays slanting down from the surface
    const qreal rays[][2] = { { 0.12, 0.09 }, { 0.30, 0.05 }, { 0.52, 0.12 }, { 0.72, 0.06 }, { 0.90, 0.09 } };
    for (int i = 0; i < 5; ++i) {
        const qreal w = rays[i][1] * screen.width();
        const qreal sway = std::sin(t * 0.5 + i * 1.7);
        p.save();
        p.translate(screen.left() + rays[i][0] * screen.width() + sway * 12, screen.top() - 40);
        p.rotate(24);
        QLinearGradient beam(0, 0, 0, screen.height() * 1.2);
        beam.setColorAt(0, QColor(200, 240, 255, int(26 + 14 * sway)));
        beam.setColorAt(1, QColor(200, 240, 255, 0));
        p.fillRect(QRectF(-w / 2, 0, w, screen.height() * 1.4), beam);
        p.restore();
    }

    // dot grid
    p.setPen(QPen(QColor(170, 225, 255, 55), 2, Qt::SolidLine, Qt::RoundCap));
    QList<QPointF> dots;
    for (qreal y = screen.top() + DotSpacing / 2; y < screen.bottom(); y += DotSpacing) {
        for (qreal x = screen.left() + DotSpacing / 2; x < screen.right(); x += DotSpacing) {
            dots.append({ x, y });
        }
    }
    p.drawPoints(dots.constData(), int(dots.size()));

    // particles rising slowly
    p.setPen(Qt::NoPen);
    for (const Speck& s : std::as_const(m_specks)) {
        const qreal y = std::fmod(s.y - s.speed * t + 10.0, 1.0);
        const qreal x = s.x + 0.01 * std::sin(t * 0.3 + s.phase);
        p.setBrush(QColor(216, 244, 255, int(255 * s.alpha * (0.7 + 0.3 * std::sin(t + s.phase)))));
        p.drawEllipse(QPointF(screen.left() + x * screen.width(), screen.top() + y * screen.height()), s.radius, s.radius);
    }

    // inner glow along the edge
    p.strokePath(shape, QPen(QColor(120, 210, 255, 70), 18));
    p.restore();

    p.strokePath(shape, QPen(QColor(215, 240, 255, 200), 2));
}
