#include "MetalWidgets.hpp"

#include "themes/winamp/Theme.hpp"

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>

namespace winamp {

// ---- capsule -------------------------------------------------------------------------------------

CapsuleButton::CapsuleButton(const QString& text, Metal::Glyph glyph, QWidget* parent)
    : QAbstractButton(parent)
    , m_glyph(glyph)
{
    setText(text);
    setFont(Metal::uiFont(8.5, true));
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    setFixedSize(sizeHint());
}

void CapsuleButton::setActive(bool active)
{
    m_active = active;
    update();
}

QSize CapsuleButton::sizeHint() const
{
    const int glyphWidth = m_glyph == Metal::Glyph::None ? 0 : 14;
    const int textWidth = text().isEmpty() ? 0 : fontMetrics().horizontalAdvance(text()) + (glyphWidth ? 6 : 0);
    return QSize(std::max(40, glyphWidth + textWidth + 26), 24);
}

void CapsuleButton::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    const bool pressed = isDown();
    Metal::capsule(p, QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), pressed, m_active, m_hover && isEnabled());
    const QColor ink = !isEnabled() ? Theme::textDim : m_active ? Qt::white : Theme::text;
    const QPointF shift = pressed ? QPointF(0, 1) : QPointF();
    const int glyphWidth = m_glyph == Metal::Glyph::None ? 0 : 14;
    const int textWidth = text().isEmpty() ? 0 : fontMetrics().horizontalAdvance(text()) + (glyphWidth ? 6 : 0);
    qreal x = (width() - glyphWidth - textWidth) / 2.0;
    if (glyphWidth) {
        Metal::glyph(p, QRectF(x, height() / 2.0 - 5, 12, 10).translated(shift), m_glyph, ink);
        x += glyphWidth + 6;
    }
    if (!text().isEmpty()) {
        p.setPen(ink);
        p.drawText(QRectF(x, 0, textWidth, height()).translated(shift), Qt::AlignVCenter | Qt::AlignLeft, text());
    }
}

void CapsuleButton::enterEvent(QEnterEvent* event)
{
    m_hover = true;
    update();
    QAbstractButton::enterEvent(event);
}

void CapsuleButton::leaveEvent(QEvent* event)
{
    m_hover = false;
    update();
    QAbstractButton::leaveEvent(event);
}

// ---- round ---------------------------------------------------------------------------------------

RoundButton::RoundButton(Metal::Glyph glyph, int diameter, QWidget* parent)
    : QAbstractButton(parent)
    , m_glyph(glyph)
{
    setFixedSize(diameter, diameter);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
}

void RoundButton::setGlyph(Metal::Glyph glyph)
{
    m_glyph = glyph;
    update();
}

void RoundButton::setActive(bool active)
{
    m_active = active;
    update();
}

void RoundButton::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    const QRectF r = QRectF(rect()).adjusted(1, 1, -1, -1);
    Metal::roundButton(p, r, isDown(), m_active, m_hover && isEnabled());
    const qreal s = r.width() * 0.34;
    const QPointF shift = isDown() ? QPointF(0.5, 1) : QPointF();
    const QColor ink = !isEnabled() ? Theme::textDim : m_active ? Theme::accent : QColor(0x24, 0x2c, 0x3c);
    Metal::glyph(p, QRectF(r.center().x() - s / 2, r.center().y() - s / 2 * 0.9, s, s * 0.9).translated(shift), m_glyph, ink);
}

void RoundButton::enterEvent(QEnterEvent* event)
{
    m_hover = true;
    update();
    QAbstractButton::enterEvent(event);
}

void RoundButton::leaveEvent(QEvent* event)
{
    m_hover = false;
    update();
    QAbstractButton::leaveEvent(event);
}

// ---- panel ---------------------------------------------------------------------------------------

LcdPanel::LcdPanel(QWidget* parent)
    : QWidget(parent)
{
}

void LcdPanel::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    Metal::lcd(p, QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), Theme::radiusPanel);
}

// ---- slider --------------------------------------------------------------------------------------

PositionSlider::PositionSlider(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(20);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setCursor(Qt::PointingHandCursor);
}

void PositionSlider::setMaximum(int maximum)
{
    m_maximum = std::max(0, maximum);
    m_value = std::min(m_value, m_maximum);
    update();
}

void PositionSlider::setValue(int value)
{
    m_value = std::clamp(value, 0, m_maximum);
    update();
}

QRectF PositionSlider::track() const
{
    return QRectF(18, height() / 2.0 - 3, width() - 36, 6);
}

QRectF PositionSlider::thumb() const
{
    const QRectF t = track();
    const qreal f = m_maximum ? qreal(m_value) / m_maximum : 0;
    return QRectF(t.left() + f * t.width() - 16, 2, 32, height() - 4);
}

void PositionSlider::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    Metal::sliderTrack(p, track(), m_maximum ? qreal(m_value) / m_maximum : 0);
    Metal::sliderThumb(p, thumb(), m_dragging);
}

void PositionSlider::seek(qreal x)
{
    const QRectF t = track();
    const int value = m_maximum ? int(std::lround(std::clamp((x - t.left()) / t.width(), 0.0, 1.0) * m_maximum)) : 0;
    if (value != m_value) {
        m_value = value;
        update();
        emit valueChanged(value);
    }
}

void PositionSlider::mousePressEvent(QMouseEvent* event)
{
    m_dragging = true;
    seek(event->position().x());
    update();
}

void PositionSlider::mouseMoveEvent(QMouseEvent* event)
{
    if (m_dragging) {
        seek(event->position().x());
    }
}

void PositionSlider::mouseReleaseEvent(QMouseEvent*)
{
    m_dragging = false;
    update();
}

namespace MetalUi {

QLabel* lcdLabel(const QString& text, bool title)
{
    auto* label = new QLabel(text);
    label->setObjectName(title ? QStringLiteral("lcdTitle") : QStringLiteral("lcd"));
    return label;
}

QScrollArea* scrollArea(QWidget* content)
{
    auto* area = new QScrollArea;
    area->setFrameShape(QFrame::NoFrame);
    area->setWidgetResizable(true);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->viewport()->setAutoFillBackground(false);
    content->setAutoFillBackground(false);
    area->setWidget(content);
    return area;
}

} // namespace MetalUi

} // namespace winamp
