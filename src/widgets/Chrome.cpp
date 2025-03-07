#include "Chrome.hpp"

#include "Icons.hpp"
#include "Theme.hpp"
#include "widgets/Metal.hpp"

#include <QMouseEvent>
#include <QPainter>
#include <QWindow>

#include <utility>

// ---- title bar ------------------------------------------------------------------------------------

TitleBar::TitleBar(const QString& title, QWidget* parent)
    : QWidget(parent)
    , m_title(title)
{
    setFixedHeight(26);
    setMouseTracking(true);
}

QRectF TitleBar::controlRect(int index) const
{
    return QRectF(width() - 8 - (3 - index) * 22 + 2, 4, 19, 17);
}

int TitleBar::controlAt(const QPointF& pos) const
{
    for (int i = 0; i < 3; ++i) {
        if (controlRect(i).contains(pos)) {
            return i;
        }
    }
    return -1;
}

void TitleBar::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    Metal::titleBar(p, QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5));

    // logo plate on the left
    const QRectF logo(6, 4, 19, 17);
    Metal::capsule(p, logo, false, false, false);
    Icons::paint(p, Icon::Image, logo.adjusted(4, 3, -4, -3), Theme::titleTop);

    // centred branding between two ridged grooves
    p.setFont(Metal::uiFont(10.5, true));
    const qreal tw = p.fontMetrics().horizontalAdvance(m_title);
    const QRectF text(width() / 2.0 - tw / 2, 0, tw, height());
    Metal::ridge(p, QRectF(logo.right() + 10, height() / 2.0 - 4, text.left() - logo.right() - 22, 8));
    Metal::ridge(p, QRectF(text.right() + 12, height() / 2.0 - 4, controlRect(0).left() - text.right() - 22, 8));
    p.setPen(QColor(0, 0, 0, 140));
    p.drawText(text.translated(1, 1), Qt::AlignCenter, m_title);
    p.setPen(Qt::white);
    p.drawText(text, Qt::AlignCenter, m_title);

    const Metal::Glyph glyphs[] = { Metal::Glyph::Minimize, Metal::Glyph::Maximize, Metal::Glyph::Close };
    for (int i = 0; i < 3; ++i) {
        const QRectF r = controlRect(i);
        Metal::capsule(p, r, i == m_pressed, false, i == m_hover);
        Metal::glyph(p, r.adjusted(5, 4, -5, -4), glyphs[i], QColor(0x24, 0x2c, 0x3c));
    }
}

void TitleBar::mousePressEvent(QMouseEvent* event)
{
    m_pressed = controlAt(event->position());
    if (m_pressed < 0 && event->button() == Qt::LeftButton && window()->windowHandle()) {
        window()->windowHandle()->startSystemMove();
    }
    update();
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event)
{
    const int pressed = std::exchange(m_pressed, -1);
    if (pressed >= 0 && pressed == controlAt(event->position())) {
        QWidget* w = window();
        if (pressed == 0) {
            w->showMinimized();
        } else if (pressed == 1) {
            w->isMaximized() ? w->showNormal() : w->showMaximized();
        } else {
            w->close();
        }
    }
    update();
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (controlAt(event->position()) < 0) {
        window()->isMaximized() ? window()->showNormal() : window()->showMaximized();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent* event)
{
    if (const int i = controlAt(event->position()); i != m_hover) {
        m_hover = i;
        update();
    }
}

void TitleBar::leaveEvent(QEvent*)
{
    m_hover = -1;
    update();
}

// ---- menu strip -----------------------------------------------------------------------------------

MenuStrip::MenuStrip(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(28);
    setMouseTracking(true);
    setFont(Metal::uiFont(10));
}

void MenuStrip::addItem(const QString& textWithMnemonic)
{
    m_items.append(textWithMnemonic);
    if (m_current < 0) {
        m_current = 0;
    }
    update();
}

void MenuStrip::setCurrentIndex(int index)
{
    if (index < 0 || index >= m_items.size() || index == m_current) {
        return;
    }
    m_current = index;
    update();
    emit currentChanged(index);
}

QChar MenuStrip::mnemonic(int index) const
{
    const QString& item = m_items.at(index);
    const qsizetype amp = item.indexOf(u'&');
    return amp >= 0 && amp + 1 < item.size() ? item.at(amp + 1).toUpper() : QChar();
}

QRectF MenuStrip::itemRect(int index) const
{
    qreal x = 8;
    for (int i = 0; i < index; ++i) {
        x += fontMetrics().horizontalAdvance(QString(m_items[i]).remove(u'&')) + 22;
    }
    return QRectF(x, 3, fontMetrics().horizontalAdvance(QString(m_items[index]).remove(u'&')) + 18, height() - 6);
}

int MenuStrip::itemAt(const QPointF& pos) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (itemRect(i).contains(pos)) {
            return i;
        }
    }
    return -1;
}

void MenuStrip::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    for (int i = 0; i < m_items.size(); ++i) {
        const QRectF r = itemRect(i);
        if (i == m_current) {
            Metal::capsule(p, r, true, false, false); // the open menu sits pressed in
        } else if (i == m_hover) {
            Metal::capsule(p, r, false, false, true);
        }
        p.setPen(i == m_current ? Theme::accent : Theme::text);
        p.drawText(r, Qt::AlignCenter | Qt::TextShowMnemonic, m_items[i]);
    }
}

void MenuStrip::mousePressEvent(QMouseEvent* event)
{
    if (const int i = itemAt(event->position()); i >= 0) {
        setCurrentIndex(i);
    }
}

void MenuStrip::mouseMoveEvent(QMouseEvent* event)
{
    const int i = itemAt(event->position());
    setCursor(i >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
    if (i != m_hover) {
        m_hover = i;
        update();
    }
}

void MenuStrip::leaveEvent(QEvent*)
{
    m_hover = -1;
    update();
}
