#include "PageTransition.hpp"

#include <QPainter>

namespace {
constexpr int DurationMs = 260;
constexpr qreal Travel = 48; // pixels each page slides
}

PageTransition::PageTransition(QWidget* parent)
    : QWidget(parent)
{
    hide();
    m_animation.setStartValue(0.0);
    m_animation.setEndValue(1.0);
    m_animation.setDuration(DurationMs);
    m_animation.setEasingCurve(QEasingCurve::OutCubic);
    connect(&m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
        m_progress = value.toReal();
        update();
    });
    connect(&m_animation, &QVariantAnimation::finished, this, &PageTransition::finish);
}

void PageTransition::run(const QPixmap& from, const QPixmap& to, int direction)
{
    m_animation.stop();
    m_from = from;
    m_to = to;
    m_direction = direction < 0 ? -1 : 1;
    m_progress = 0;
    show();
    raise();
    m_animation.start();
}

bool PageTransition::isRunning() const
{
    return isVisible();
}

void PageTransition::finish()
{
    if (!isVisible()) {
        return;
    }
    m_animation.stop();
    hide();
    m_from = m_to = QPixmap();
    emit finished();
}

void PageTransition::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    const qreal t = m_progress;
    p.setOpacity(1 - t);
    p.drawPixmap(QPointF(-m_direction * Travel * t, 0), m_from);
    p.setOpacity(t);
    p.drawPixmap(QPointF(m_direction * Travel * (1 - t), 0), m_to);
}
