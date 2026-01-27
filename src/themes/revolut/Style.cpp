#include "themes/revolut/Style.hpp"

#include "themes/revolut/Theme.hpp"

#include <QPainter>
#include <QPainterPath>

namespace revolut {

namespace {

    QColor alpha(QColor c, int a)
    {
        c.setAlpha(a);
        return c;
    }

    QPainterPath rounded(const QRectF& r, qreal radius)
    {
        QPainterPath path;
        const qreal rr = std::min(radius, std::min(r.width(), r.height()) / 2);
        path.addRoundedRect(r, rr, rr);
        return path;
    }

    void fill(QPainter& p, const QRectF& r, qreal radius, const QColor& color)
    {
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.fillPath(rounded(r, radius), color);
        p.restore();
    }

    void stroke(QPainter& p, const QRectF& r, qreal radius, const QColor& color, qreal width)
    {
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        const qreal in = width / 2;
        p.strokePath(rounded(r.adjusted(in, in, -in, -in), radius - in), QPen(color, width));
        p.restore();
    }

    /// --rui-shadow-level*: a soft shadow falling slightly below the shape.
    void shadow(QPainter& p, const QRectF& r, qreal radius, int level)
    {
        const int spread = 2 + 2 * level;
        for (int i = spread; i > 0; --i) {
            fill(p, r.adjusted(-i, -i + level, i, i + level), radius + i, QColor(26, 15, 51, 5 + level));
        }
    }

    /// Amber keyboard-focus ring just outside the shape.
    void focusRing(QPainter& p, const QRectF& r, qreal radius)
    {
        stroke(p, r.adjusted(-1, -1, 1, 1), radius + 1, Theme::focus, 2);
    }

    const QColor disabledFill { 0xef, 0xec, 0xf4 };
    const QColor disabledText { 0xb3, 0xad, 0xc4 };
    const QColor track = QColor::fromHslF(266 / 360.0f, 0.30f, 0.88f);
    const QColor selected = QColor::fromHslF(266 / 360.0f, 1.00f, 0.92f);

} // namespace

Style::Style()
    : WidgetStyle([] {
        Metrics m;
        m.control = 38;
        m.padding = 20;
        m.frame = 2;
        m.indicator = 20;
        m.arrow = 10;
        m.groove = 6;
        m.scroll = 8;
        m.tab = 36;
        m.title = 44;
        m.row = 32;
        m.margin = 20;
        m.spacing = 12;
        return m;
    }())
{
}

QFont Style::font() const
{
    return Theme::font();
}

QPalette Style::standardPalette() const
{
    QPalette pal;
    pal.setColor(QPalette::Window, Theme::canvas);
    pal.setColor(QPalette::WindowText, Theme::text);
    pal.setColor(QPalette::Base, Qt::transparent); // fields and views draw their white widget
    pal.setColor(QPalette::AlternateBase, QColor(0xf9, 0xf6, 0xfe));
    pal.setColor(QPalette::Text, Theme::text);
    pal.setColor(QPalette::PlaceholderText, Theme::textTertiary);
    pal.setColor(QPalette::Button, Theme::chip);
    pal.setColor(QPalette::ButtonText, Theme::text);
    pal.setColor(QPalette::BrightText, Theme::focus);
    pal.setColor(QPalette::Highlight, selected);
    pal.setColor(QPalette::HighlightedText, Theme::text);
    pal.setColor(QPalette::ToolTipBase, Theme::text);
    pal.setColor(QPalette::ToolTipText, Theme::stageText);
    pal.setColor(QPalette::Link, Theme::accentHover);
    pal.setColor(QPalette::Light, Theme::widget);
    pal.setColor(QPalette::Midlight, Theme::surface);
    pal.setColor(QPalette::Mid, Theme::hairline);
    pal.setColor(QPalette::Dark, Theme::textTertiary);
    pal.setColor(QPalette::Shadow, Theme::text);
    for (const QPalette::ColorRole role : { QPalette::WindowText, QPalette::Text, QPalette::ButtonText }) {
        pal.setColor(QPalette::Disabled, role, disabledText);
    }
    return pal;
}

void Style::window(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, Theme::canvas);
}

void Style::card(QPainter& p, const QWidget*, const QRect& rect, const QRect& titleRect, const QString& title) const
{
    // each card owns one accent: the next pair of the rotation, in order of appearance
    int index = m_accents.value(title, -1);
    if (index < 0) {
        index = int(m_accents.size() % std::size(Theme::tints));
        m_accents.insert(title, index);
    }
    const Theme::Tint& tint = Theme::tints[index];
    fill(p, QRectF(rect), Theme::radiusCard, tint.tint);
    if (title.isEmpty()) {
        return;
    }
    // eyebrow: small, bold, uppercase, in the card's accent
    QFont f = Theme::font(8.25, QFont::Bold);
    f.setCapitalization(QFont::AllUppercase);
    f.setLetterSpacing(QFont::AbsoluteSpacing, 0.8);
    p.save();
    p.setFont(f);
    p.setPen(tint.accent);
    p.drawText(titleRect.adjusted(22, 14, -22, 0), Qt::AlignLeft | Qt::AlignVCenter, title);
    p.restore();
}

void Style::button(QPainter& p, const QRect& rect, Button kind, const Look& look) const
{
    const QRectF r = QRectF(rect).adjusted(1, 1, -1, -1);
    const qreal radius = r.height() / 2; // always pill
    if (kind == Button::Flat) {
        if (look.hover || look.pressed || look.checked) {
            fill(p, r, radius, look.pressed ? Theme::chip.darker(106) : alpha(Theme::chip, 200));
        }
    } else if (!look.enabled) {
        fill(p, r, radius, disabledFill);
    } else if (kind == Button::Default || look.checked) {
        fill(p, r, radius, look.pressed ? Theme::accentHover.darker(112) : look.hover ? Theme::accentHover : Theme::accent);
    } else {
        fill(p, r, radius, look.pressed ? Theme::chip.darker(110) : look.hover ? Theme::chip.darker(104) : Theme::chip);
    }
    if (look.focus) {
        focusRing(p, r, radius);
    }
}

QColor Style::buttonText(Button kind, const Look& look, const QPalette&) const
{
    if (!look.enabled) {
        return disabledText;
    }
    return kind != Button::Flat && (kind == Button::Default || look.checked) ? Qt::white : Theme::text;
}

void Style::field(QPainter& p, const QRect& rect, const Look& look) const
{
    const QRectF r = QRectF(rect).adjusted(1, 1, -1, -1);
    fill(p, r, Theme::radiusField, look.enabled ? Theme::widget : disabledFill);
    if (look.focus) {
        focusRing(p, r, Theme::radiusField);
    } else {
        stroke(p, r, Theme::radiusField, look.hover ? Theme::accent : Theme::hairline, 1);
    }
}

void Style::view(QPainter& p, const QRect& rect, const Look& look) const
{
    const QRectF r(rect);
    fill(p, r, Theme::radiusWidget, look.enabled ? Theme::widget : disabledFill);
    stroke(p, r, Theme::radiusWidget, Theme::hairline, 1);
}

void Style::check(QPainter& p, const QRect& rect, Qt::CheckState state, const Look& look) const
{
    const QRectF r = QRectF(rect).adjusted(1, 1, -1, -1);
    if (state == Qt::Unchecked) {
        fill(p, r, 6, look.enabled ? Theme::widget : disabledFill);
        stroke(p, r, 6, look.hover ? Theme::accent : QColor(0xc4, 0xbd, 0xd4), 1.5);
    } else {
        fill(p, r, 6, look.enabled ? (look.pressed ? Theme::accentHover : Theme::accent) : disabledText);
        const auto at = [&](qreal x, qreal y) { return QPointF(r.left() + x * r.width(), r.top() + y * r.height()); };
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        if (state == Qt::Checked) {
            p.drawPolyline(QPolygonF({ at(0.26, 0.52), at(0.44, 0.69), at(0.75, 0.33) }));
        } else {
            p.drawLine(at(0.28, 0.5), at(0.72, 0.5));
        }
        p.restore();
    }
    if (look.focus) {
        focusRing(p, r, 6);
    }
}

void Style::radio(QPainter& p, const QRect& rect, bool on, const Look& look) const
{
    const QRectF r = QRectF(rect).adjusted(1, 1, -1, -1);
    const qreal radius = r.height() / 2;
    if (on) {
        // a thick accent ring round a white centre
        fill(p, r, radius, look.enabled ? (look.pressed ? Theme::accentHover : Theme::accent) : disabledText);
        const qreal in = r.width() * 0.3;
        fill(p, r.adjusted(in, in, -in, -in), radius, Theme::widget);
    } else {
        fill(p, r, radius, look.enabled ? Theme::widget : disabledFill);
        stroke(p, r, radius, look.hover ? Theme::accent : QColor(0xc4, 0xbd, 0xd4), 1.5);
    }
    if (look.focus) {
        focusRing(p, r, radius);
    }
}

void Style::arrow(QPainter& p, const QRect& rect, Qt::ArrowType type, const Look& look) const
{
    const QRectF r(rect);
    const auto at = [&](qreal x, qreal y) { return QPointF(r.left() + x * r.width(), r.top() + y * r.height()); };
    QPolygonF line;
    switch (type) {
    case Qt::UpArrow: line = { at(0.1, 0.68), at(0.5, 0.3), at(0.9, 0.68) }; break;
    case Qt::DownArrow: line = { at(0.1, 0.32), at(0.5, 0.7), at(0.9, 0.32) }; break;
    case Qt::LeftArrow: line = { at(0.68, 0.1), at(0.3, 0.5), at(0.68, 0.9) }; break;
    case Qt::RightArrow: line = { at(0.32, 0.1), at(0.7, 0.5), at(0.32, 0.9) }; break;
    default: return;
    }
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(!look.enabled ? disabledText : look.hover ? Theme::accent : Theme::textSecondary, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPolyline(line);
    p.restore();
}

void Style::groove(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation, const Look& look) const
{
    const QRectF r(rect);
    const qreal radius = std::min(r.width(), r.height()) / 2;
    fill(p, r, radius, track);
    if (!filled.isEmpty()) {
        fill(p, QRectF(filled), radius, look.enabled ? Theme::accent : disabledText);
    }
}

void Style::handle(QPainter& p, const QRect& rect, Qt::Orientation, const Look& look) const
{
    const QRectF r = QRectF(rect).adjusted(2, 2, -2, -2);
    const qreal radius = r.height() / 2;
    if (look.enabled) {
        shadow(p, r, radius, look.hover || look.pressed ? 2 : 1);
    }
    fill(p, r, radius, look.enabled ? Theme::widget : disabledFill);
    stroke(p, r, radius, look.pressed ? Theme::accent : Theme::hairline, look.pressed ? 2 : 1);
    if (look.focus) {
        focusRing(p, r, radius);
    }
}

QSize Style::handleSize(Qt::Orientation) const
{
    return { 24, 24 };
}

void Style::scrollBar(QPainter& p, const QRect&, const QRect& handle, Qt::Orientation orientation, const Look& look) const
{
    const QRectF r = orientation == Qt::Horizontal ? QRectF(handle).adjusted(2, 1, -2, -1) : QRectF(handle).adjusted(1, 2, -1, -2);
    fill(p, r, std::min(r.width(), r.height()) / 2, alpha(Theme::text, look.pressed ? 140 : look.hover ? 100 : 55));
}

void Style::progress(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation) const
{
    const QRectF r(rect);
    const qreal radius = std::min(r.width(), r.height()) / 2;
    fill(p, r, radius, track);
    if (!filled.isEmpty()) {
        // the pill keeps its round ends even when barely filled
        p.save();
        p.setClipPath(rounded(r, radius));
        fill(p, QRectF(filled).adjusted(filled.width() < filled.height() ? -filled.height() : 0, 0, 0, 0), radius, Theme::accent);
        p.restore();
    }
}

void Style::tab(QPainter& p, const QRect& rect, bool selected, const Look& look) const
{
    // segmented pills: the selected one is a white widget lifted off the card
    const QRectF r = QRectF(rect).adjusted(2, 3, -2, -3);
    const qreal radius = r.height() / 2;
    if (selected) {
        shadow(p, r, radius, 1);
        fill(p, r, radius, Theme::widget);
    } else if (look.hover) {
        fill(p, r, radius, alpha(Theme::chip, 220));
    }
    if (look.focus) {
        focusRing(p, r, radius);
    }
}

void Style::tabPane(QPainter&, const QWidget*, const QRect&) const
{
}

void Style::dial(QPainter& p, const QRect& rect, qreal value, const Look& look) const
{
    const QRectF ring = QRectF(rect).adjusted(8, 8, -8, -8);
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(track, 8, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(ring, 225 * 16, -270 * 16);
    if (value > 0) {
        p.setPen(QPen(look.enabled ? Theme::accent : disabledText, 8, Qt::SolidLine, Qt::RoundCap));
        p.drawArc(ring, 225 * 16, int(-270 * 16 * value));
    }
    p.restore();

    const QRectF knob = ring.adjusted(ring.width() * 0.22, ring.height() * 0.22, -ring.width() * 0.22, -ring.height() * 0.22);
    if (look.enabled) {
        shadow(p, knob, knob.height() / 2, look.hover || look.pressed ? 3 : 2);
    }
    fill(p, knob, knob.height() / 2, look.enabled ? Theme::widget : disabledFill);
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(look.enabled ? Theme::accent : disabledText);
    p.drawEllipse(dialPoint(knob, value, knob.width() * 0.3), 4, 4);
    p.restore();
    if (look.focus) {
        focusRing(p, knob, knob.height() / 2);
    }
}

void Style::display(QPainter& p, const QRect& rect) const
{
    // the dark stage with its violet glow
    const QRectF r(rect);
    fill(p, r, Theme::radiusWidget, Theme::stage);
    QRadialGradient glow(QPointF(r.center().x(), r.bottom()), r.width() * 0.6);
    glow.setColorAt(0, alpha(Theme::accent, 110));
    glow.setColorAt(1, alpha(Theme::accent, 0));
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.fillPath(rounded(r, Theme::radiusWidget), glow);
    p.restore();
}

QColor Style::displayText() const
{
    return Theme::stageText;
}

void Style::menuBar(QPainter& p, const QWidget*, const QRect& rect) const
{
    // the navigation bar, solid once the page scrolls: white with a hairline
    p.fillRect(rect, Theme::widget);
    p.fillRect(QRect(rect.left(), rect.bottom(), rect.width(), 1), Theme::hairline);
}

QColor Style::menuBarText(bool active) const
{
    return active ? Theme::accent : Theme::text;
}

void Style::menu(QPainter& p, const QRect& rect) const
{
    p.fillRect(rect, Theme::widget);
    p.setPen(Theme::hairline);
    p.drawRect(QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5));
}

void Style::highlight(QPainter& p, const QRect& rect, bool inBar) const
{
    if (inBar) {
        fill(p, QRectF(rect).adjusted(0, 3, 0, -3), (rect.height() - 6) / 2.0, alpha(Theme::chip, 230));
    } else {
        fill(p, QRectF(rect).adjusted(4, 1, -4, -1), 8, selected);
    }
}

void Style::selection(QPainter& p, const QRect& rect, const Look& look) const
{
    fill(p, QRectF(rect).adjusted(2, 1, -2, -1), 10, look.checked ? selected : QColor(0xf6, 0xf2, 0xfd));
}

void Style::header(QPainter& p, const QRect& rect, const Look& look) const
{
    p.fillRect(rect, look.hover ? QColor(0xf9, 0xf6, 0xfe) : Theme::widget);
    p.fillRect(QRect(rect.left(), rect.bottom(), rect.width(), 1), Theme::hairline);
}

void Style::tooltip(QPainter& p, const QRect& rect) const
{
    p.fillRect(rect, Theme::text);
}

} // namespace revolut
