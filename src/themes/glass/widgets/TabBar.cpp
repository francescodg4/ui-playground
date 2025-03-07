#include "TabBar.hpp"

#include "themes/glass/Theme.hpp"
#include "themes/glass/widgets/Glass.hpp"

#include <QHelpEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QToolTip>

#include <utility>

namespace glass {

namespace {
constexpr qreal TabWidth = 60;
constexpr qreal TabHeight = 34;
constexpr qreal TabGap = 6;
constexpr qreal BarPad = 6; // glass bar around the tabs
constexpr qreal TopMargin = 14; // room for badges and glow
}

TabBar::TabBar(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void TabBar::addTab(Icon icon, const QString& toolTip)
{
    m_tabs.append({ icon, toolTip });
    if (m_current < 0) {
        m_current = 0;
    }
    updateGeometry();
    update();
}

void TabBar::setBadge(int index, int count)
{
    if (index >= 0 && index < m_tabs.size()) {
        m_tabs[index].badge = count;
        update();
    }
}

int TabBar::badge(int index) const
{
    return index >= 0 && index < m_tabs.size() ? m_tabs[index].badge : 0;
}

void TabBar::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_tabs.size()) {
        return;
    }
    m_tabs[index].badge = 0; // visiting a tab acknowledges its notifications
    if (index != m_current) {
        m_current = index;
        emit currentChanged(index);
    }
    update();
}

QSize TabBar::sizeHint() const
{
    const qreal width = m_tabs.size() * (TabWidth + TabGap) + 2 * TabHeight;
    return QSize(int(width), int(TopMargin + TabHeight + 2 * BarPad + 16));
}

QRectF TabBar::tabRect(int index) const
{
    const qreal total = m_tabs.size() * TabWidth + (m_tabs.size() - 1) * TabGap;
    const qreal x0 = (width() - total) / 2;
    return QRectF(x0 + index * (TabWidth + TabGap), TopMargin + BarPad, TabWidth, TabHeight);
}

int TabBar::tabAt(const QPointF& pos) const
{
    for (int i = 0; i < m_tabs.size(); ++i) {
        if (tabRect(i).adjusted(-TabGap / 2, 0, TabGap / 2, 0).contains(pos)) {
            return i;
        }
    }
    return -1;
}

void TabBar::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    if (m_tabs.isEmpty()) {
        return;
    }

    // floating liquid-glass bar
    const QRectF bar = tabRect(0).united(tabRect(int(m_tabs.size()) - 1)).adjusted(-BarPad, -BarPad, BarPad, BarPad);
    Glass::paintSurface(p, this, bar, bar.height() / 2, Glass::Level::Bar);

    for (int i = 0; i < m_tabs.size(); ++i) {
        const QRectF r = tabRect(i);
        const bool current = i == m_current;
        const bool pressed = i == m_pressed;
        const qreal radius = r.height() / 2;

        if (current) {
            Glass::paintGlow(p, r, radius, Theme::glowAccent);
            Glass::paintClay(p, r, radius, Theme::accent, pressed);
        } else if (pressed) {
            Glass::paintClay(p, r, radius, Theme::clay, true);
        } else if (i == m_hover) {
            Glass::paintGlow(p, r, radius, Theme::glowSoft, 0.6);
            QPainterPath pill;
            pill.addRoundedRect(r, radius, radius);
            p.fillPath(pill, QColor(255, 255, 255, 16));
        }

        const qreal s = 18 * (pressed ? Theme::pressedScale : 1.0);
        Icons::paint(p, m_tabs[i].icon, QRectF(r.center().x() - s / 2, r.center().y() - s / 2, s, s),
            current ? Qt::white : Theme::text);

        if (const int count = m_tabs[i].badge; count > 0) {
            const QRectF b(r.right() - 16, r.top() - 9, 18, 18);
            Glass::paintGlow(p, b, 9, Theme::glowBadge);
            Glass::paintClay(p, b, 9, Theme::clayBadge);
            QFont f = font();
            f.setBold(true);
            f.setPixelSize(10);
            p.setFont(f);
            p.setPen(QColor(0x3b, 0x24, 0x00));
            p.drawText(b, Qt::AlignCenter, QString::number(count));
        }
    }
}

bool TabBar::event(QEvent* event)
{
    if (event->type() == QEvent::ToolTip) {
        auto* help = static_cast<QHelpEvent*>(event);
        const int index = tabAt(help->pos());
        if (index >= 0) {
            QToolTip::showText(help->globalPos(), m_tabs[index].toolTip, this);
        } else {
            QToolTip::hideText();
        }
        return true;
    }
    return QWidget::event(event);
}

void TabBar::mousePressEvent(QMouseEvent* event)
{
    m_pressed = tabAt(event->position());
    update();
}

void TabBar::mouseReleaseEvent(QMouseEvent* event)
{
    const int pressed = std::exchange(m_pressed, -1);
    if (pressed >= 0 && pressed == tabAt(event->position())) {
        setCurrentIndex(pressed);
    }
    update();
}

void TabBar::mouseMoveEvent(QMouseEvent* event)
{
    if (const int index = tabAt(event->position()); index != m_hover) {
        m_hover = index;
        update();
    }
}

void TabBar::leaveEvent(QEvent*)
{
    m_hover = -1;
    update();
}

} // namespace glass
