#include "ClayButton.hpp"

#include "Theme.hpp"
#include "widgets/Glass.hpp"

#include <QPainter>

namespace {
constexpr int Margin = 16; // room for the glow and the soft shadow around the body
}

ClayButton::ClayButton(Icon icon, QWidget* parent)
    : QAbstractButton(parent)
    , m_icon(icon)
    , m_glow(Theme::glowSoft)
{
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    setFixedSize(sizeHint());
    connect(&m_frames, &QTimer::timeout, this, qOverload<>(&QWidget::update));
}

void ClayButton::setIconShape(Icon icon)
{
    m_icon = icon;
    update();
}

void ClayButton::setIconColor(const QColor& color)
{
    m_iconColor = color;
    update();
}

void ClayButton::setGlowColor(const QColor& color)
{
    m_glow = color;
    update();
}

void ClayButton::setBodySize(const QSize& size)
{
    m_body = size;
    setFixedSize(sizeHint());
    update();
}

void ClayButton::setActive(bool active)
{
    m_active = active;
    if (active && !Glass::settings().reducedMotion) {
        m_frames.start(33);
    } else {
        m_frames.stop();
    }
    update();
}

QSize ClayButton::sizeHint() const
{
    const int textWidth = text().isEmpty() ? 0 : fontMetrics().horizontalAdvance(text()) + 10;
    return QSize(m_body.width() + textWidth + 2 * Margin, m_body.height() + 2 * Margin);
}

QRectF ClayButton::bodyRect() const
{
    return QRectF(rect()).adjusted(Margin, Margin, -Margin, -Margin);
}

void ClayButton::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    const QRectF body = bodyRect();
    const qreal radius = body.height() / 2; // fully rounded
    const bool pressed = isDown();

    if (!isEnabled()) {
        p.setOpacity(0.35);
    }
    if (m_active) {
        Glass::paintGlow(p, body, radius, m_glow, 0.55 + 0.45 * Glass::pulse());
    } else if (m_hover && isEnabled()) {
        Glass::paintGlow(p, body, radius, m_glow, 0.8);
    }
    Glass::paintClay(p, body, radius, Theme::clay, pressed);
    if (m_active) {
        Glass::paintSweep(p, body, radius, m_iconColor);
    }

    const QRectF face = pressed ? Glass::scaled(body, Theme::pressedScale) : body;
    const qreal s = std::min(face.height() * 0.5, 22.0);
    QRectF iconRect(face.left() + (face.height() - s) / 2, face.center().y() - s / 2, s, s);
    if (text().isEmpty()) {
        iconRect.moveCenter(face.center());
    }
    Icons::paint(p, m_icon, iconRect, m_iconColor);
    if (!text().isEmpty()) {
        p.setPen(Theme::text);
        p.drawText(face.adjusted(iconRect.right() - face.left() + 6, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, text());
    }
}

void ClayButton::enterEvent(QEnterEvent* event)
{
    m_hover = true;
    update();
    QAbstractButton::enterEvent(event);
}

void ClayButton::leaveEvent(QEvent* event)
{
    m_hover = false;
    update();
    QAbstractButton::leaveEvent(event);
}
