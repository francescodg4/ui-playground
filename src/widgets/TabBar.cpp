#include "TabBar.hpp"

#include "Theme.hpp"

#include <QHelpEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QToolTip>

namespace {
constexpr qreal TabWidth = 68;
constexpr qreal TabHeight = 30;
constexpr qreal TabGap = 10;
constexpr qreal TopMargin = 10; // room for the badges
constexpr qreal Slant = -0.32; // horizontal shear of the tab plates
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
    return QSize(int(width), int(TopMargin + TabHeight + 14));
}

QRectF TabBar::tabRect(int index) const
{
    const qreal total = m_tabs.size() * TabWidth + (m_tabs.size() - 1) * TabGap;
    const qreal x0 = (width() - total) / 2;
    return QRectF(x0 + index * (TabWidth + TabGap), TopMargin, TabWidth, TabHeight);
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

    for (int i = 0; i < m_tabs.size(); ++i) {
        const QRectF r = tabRect(i);
        const bool current = i == m_current;

        QPainterPath plate;
        plate.addRoundedRect(QRectF(-r.width() / 2, -r.height() / 2, r.width(), r.height()), 7, 7);
        plate = QTransform().translate(r.center().x(), r.center().y()).shear(Slant, 0).map(plate);

        if (current) {
            p.strokePath(plate, QPen(QColor(70, 170, 255, 110), 8));
            QLinearGradient fill(r.topLeft(), r.bottomLeft());
            fill.setColorAt(0, QColor(0x3a, 0x9b, 0xff));
            fill.setColorAt(1, QColor(0x0f, 0x63, 0xd8));
            p.fillPath(plate, fill);
            p.strokePath(plate, QPen(QColor(0xcf, 0xe8, 0xff), 1.5));
            // indicator dash under the selected tab
            p.setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap));
            p.drawLine(QPointF(r.center().x() - 5, r.bottom() + 7), QPointF(r.center().x() + 5, r.bottom() + 7));
        } else {
            p.fillPath(plate, i == m_hover ? QColor(40, 120, 200, 120) : QColor(10, 55, 100, 90));
            p.strokePath(plate, QPen(QColor(225, 242, 255, 205), 1.5));
        }

        const qreal s = 19;
        Icons::paint(p, m_tabs[i].icon, QRectF(r.center().x() - s / 2, r.center().y() - s / 2, s, s), QColor(0xe9, 0xf5, 0xff));

        if (const int count = m_tabs[i].badge; count > 0) {
            const QPointF c(r.center().x() + 13, r.top());
            QRadialGradient g(c - QPointF(2, 2), 9);
            g.setColorAt(0, QColor(0xff, 0xd9, 0x8a));
            g.setColorAt(1, Theme::badge);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(255, 180, 60, 80));
            p.drawEllipse(c, 10, 10);
            p.setBrush(g);
            p.drawEllipse(c, 7.5, 7.5);
            QFont f = font();
            f.setBold(true);
            f.setPixelSize(10);
            p.setFont(f);
            p.setPen(QColor(0x3b, 0x24, 0x00));
            p.drawText(QRectF(c.x() - 8, c.y() - 8, 16, 16), Qt::AlignCenter, QString::number(count));
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
    if (const int index = tabAt(event->position()); index >= 0) {
        setCurrentIndex(index);
    }
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
