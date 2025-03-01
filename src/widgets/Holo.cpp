#include "Holo.hpp"

#include "Theme.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>

SectionTitle::SectionTitle(const QString& text, QWidget* parent)
    : QWidget(parent)
    , m_text(text)
{
    setFont(Theme::titleFont());
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

QSize SectionTitle::sizeHint() const
{
    return QSize(fontMetrics().horizontalAdvance(m_text) + 40, fontMetrics().height() + 14);
}

void SectionTitle::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    const QFontMetricsF fm(font());
    const qreal textWidth = fm.horizontalAdvance(m_text);
    const qreal cx = width() / 2.0;

    p.setPen(Theme::cyan);
    p.drawText(QRectF(0, 0, width(), fm.height() + 2), Qt::AlignCenter, m_text);

    // gold underline: dips at the left end, flicks up at the right end
    const qreal x0 = cx - textWidth / 2 - 14;
    const qreal x1 = cx + textWidth / 2 + 14;
    const qreal y = fm.height() + 6;
    QPainterPath swoosh;
    swoosh.moveTo(x0, y + 3);
    swoosh.quadTo(x0 + 3, y, x0 + 10, y);
    swoosh.lineTo(x1 - 8, y);
    swoosh.quadTo(x1 - 3, y, x1, y - 4);
    p.setPen(QPen(Theme::gold, 1.6, Qt::SolidLine, Qt::RoundCap));
    p.drawPath(swoosh);
}

namespace Holo {

QVBoxLayout* pageLayout(QWidget* page, const QString& title)
{
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->addWidget(new SectionTitle(title), 0, Qt::AlignHCenter);
    return layout;
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

QToolButton* roundButton(Icon icon, const QString& toolTip, QWidget* parent)
{
    auto* button = new QToolButton(parent);
    button->setObjectName(QStringLiteral("round"));
    button->setIcon(Icons::icon(icon));
    button->setIconSize(QSize(20, 20));
    button->setFixedSize(36, 36);
    button->setToolTip(toolTip);
    button->setCursor(Qt::PointingHandCursor);
    button->setFocusPolicy(Qt::NoFocus);
    return button;
}

} // namespace Holo
