#include "BoxHeader.hpp"

#include "themes/emerald/Theme.hpp"

#include <QHelpEvent>
#include <QMouseEvent>
#include <QToolTip>

#include <utility>

namespace emerald {

using Pixel::Scale;

namespace {
constexpr int Tile = 24;
constexpr int TileGap = 4;
constexpr int TileTop = 16; // room for the glove above the tiles
constexpr int BannerTop = 46;
constexpr int BannerHeight = 16;
constexpr int BannerWidth = 220;
}

BoxHeader::BoxHeader(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void BoxHeader::addTab(Icon icon, const QString& name, Pixel::Wallpaper wallpaper)
{
    m_tabs.append({ icon, name, wallpaper, 0 });
    if (m_current < 0) {
        m_current = 0;
    }
    update();
}

void BoxHeader::setBadge(int index, int count)
{
    if (index >= 0 && index < m_tabs.size()) {
        m_tabs[index].badge = count;
        update();
    }
}

void BoxHeader::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_tabs.size()) {
        return;
    }
    m_tabs[index].badge = 0;
    if (index != m_current) {
        m_current = index;
        emit currentChanged(index);
    }
    update();
}

QSize BoxHeader::sizeHint() const
{
    return QSize((BannerWidth + 60) * Scale, (BannerTop + BannerHeight + 4) * Scale);
}

QRect BoxHeader::tileRect(int index) const
{
    const int total = int(m_tabs.size()) * Tile + (int(m_tabs.size()) - 1) * TileGap;
    const int x0 = (width() / Scale - total) / 2;
    return QRect(x0 + index * (Tile + TileGap), TileTop, Tile, Tile);
}

QRect BoxHeader::bannerRect() const
{
    return QRect((width() / Scale - BannerWidth) / 2, BannerTop, BannerWidth, BannerHeight);
}

QRect BoxHeader::arrowRect(bool next) const
{
    const QRect b = bannerRect();
    return next ? QRect(b.right() + 6, b.top() + 3, 8, 10) : QRect(b.left() - 14, b.top() + 3, 8, 10);
}

int BoxHeader::hit(const QPoint& devicePos) const
{
    const QPoint pos = devicePos / Scale;
    for (int i = 0; i < m_tabs.size(); ++i) {
        if (tileRect(i).contains(pos)) {
            return i;
        }
    }
    if (arrowRect(false).adjusted(-3, -3, 3, 3).contains(pos)) {
        return PrevArrow;
    }
    if (arrowRect(true).adjusted(-3, -3, 3, 3).contains(pos)) {
        return NextArrow;
    }
    return None;
}

void BoxHeader::paintEvent(QPaintEvent*)
{
    if (m_tabs.isEmpty()) {
        return;
    }
    Pixel::Canvas canvas(this);
    QPainter& p = canvas.p();

    for (int i = 0; i < m_tabs.size(); ++i) {
        const Tab& tab = m_tabs[i];
        const bool pressed = i == m_pressed;
        QRect r = tileRect(i);
        if (!pressed) {
            Pixel::chamfer(p, r.translated(1, 1), Theme::borderDark, Theme::borderDark, 2); // hard shadow
        } else {
            r.translate(1, 1);
        }
        if (i == m_current) {
            Pixel::highlightTile(p, r);
        } else {
            Pixel::chamfer(p, r, i == m_hover ? Theme::white : Theme::cream, Theme::borderDark, 2);
        }
        p.drawImage(r.left() + 4, r.top() + 4, Pixel::sprite(tab.icon, 16, i == m_current ? Theme::white : Theme::primaryGreen));

        if (tab.badge > 0) {
            const QRect b(r.right() - 5, r.top() - 3, 9, 9);
            Pixel::chamfer(p, b, Theme::badge, Theme::borderDark, 2);
            Pixel::text(p, b.adjusted(1, 0, 0, 0), Qt::AlignCenter, QString::number(tab.badge), Theme::lightText, Theme::badge.darker(160));
        }
    }
    // the glove points at the current tile
    const QRect cur = tileRect(m_current);
    Pixel::hand(p, QPoint(cur.center().x(), cur.top() - 1), Pixel::Direction::Down);

    const QRect banner = bannerRect();
    Pixel::banner(p, banner, m_tabs[m_current].wallpaper, m_tabs[m_current].name.toUpper());
    Pixel::arrow(p, arrowRect(false).translated(m_pressed == PrevArrow ? QPoint(-1, 0) : QPoint()), Pixel::Direction::Left);
    Pixel::arrow(p, arrowRect(true).translated(m_pressed == NextArrow ? QPoint(1, 0) : QPoint()), Pixel::Direction::Right);
}

bool BoxHeader::event(QEvent* event)
{
    if (event->type() == QEvent::ToolTip) {
        auto* help = static_cast<QHelpEvent*>(event);
        const int target = hit(help->pos());
        if (target >= 0) {
            QToolTip::showText(help->globalPos(), m_tabs[target].name, this);
        } else if (target == PrevArrow || target == NextArrow) {
            QToolTip::showText(help->globalPos(), target == PrevArrow ? tr("Previous (Q)") : tr("Next (E)"), this);
        } else {
            QToolTip::hideText();
        }
        return true;
    }
    return QWidget::event(event);
}

void BoxHeader::mousePressEvent(QMouseEvent* event)
{
    m_pressed = hit(event->position().toPoint());
    update();
}

void BoxHeader::mouseReleaseEvent(QMouseEvent* event)
{
    const int pressed = std::exchange(m_pressed, int(None));
    if (pressed != None && pressed == hit(event->position().toPoint())) {
        const int n = int(m_tabs.size());
        if (pressed == PrevArrow) {
            setCurrentIndex((m_current + n - 1) % n);
        } else if (pressed == NextArrow) {
            setCurrentIndex((m_current + 1) % n);
        } else {
            setCurrentIndex(pressed);
        }
    }
    update();
}

void BoxHeader::mouseMoveEvent(QMouseEvent* event)
{
    const int target = hit(event->position().toPoint());
    setCursor(target != None ? Qt::PointingHandCursor : Qt::ArrowCursor);
    if (target != m_hover) {
        m_hover = target;
        update();
    }
}

void BoxHeader::leaveEvent(QEvent*)
{
    m_hover = None;
    update();
}

} // namespace emerald
