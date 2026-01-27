#include "themes/flat/Style.hpp"

#include "themes/flat/Theme.hpp"

#include <QPainter>
#include <QPainterPath>

namespace flat {

namespace {

    /// Flat panel: solid fill and a 1px hairline, at most 2px of rounding.
    void panel(QPainter& p, const QRect& rect, const QColor& fill, const QColor& line)
    {
        const QRectF r = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(line.isValid() ? QPen(line, 1) : QPen(Qt::NoPen));
        p.setBrush(fill.isValid() ? QBrush(fill) : QBrush(Qt::NoBrush));
        p.drawRoundedRect(r, Theme::radius, Theme::radius);
        p.restore();
    }

    void chevron(QPainter& p, const QRect& rect, Qt::ArrowType type, const QColor& color)
    {
        const QRectF r(rect);
        const auto at = [&](qreal x, qreal y) { return QPointF(r.left() + x * r.width(), r.top() + y * r.height()); };
        QPolygonF line;
        switch (type) {
        case Qt::UpArrow: line = { at(0, 0.7), at(0.5, 0.3), at(1, 0.7) }; break;
        case Qt::DownArrow: line = { at(0, 0.3), at(0.5, 0.7), at(1, 0.3) }; break;
        case Qt::LeftArrow: line = { at(0.7, 0), at(0.3, 0.5), at(0.7, 1) }; break;
        case Qt::RightArrow: line = { at(0.3, 0), at(0.7, 0.5), at(0.3, 1) }; break;
        default: return;
        }
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(color, 1.4, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        p.drawPolyline(line);
        p.restore();
    }

} // namespace

Style::Style()
    : WidgetStyle([] {
        Metrics m;
        m.control = 28;
        m.padding = 12;
        m.frame = 1;
        m.indicator = 16;
        m.arrow = 8;
        m.groove = 3;
        m.scroll = 6;
        m.tab = 30;
        m.title = 38;
        m.row = 22; // --flat-row-height
        m.margin = 12; // --flat-panel-padding
        m.spacing = 8;
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
    pal.setColor(QPalette::Base, Qt::transparent); // fields and views draw their sunken surface
    pal.setColor(QPalette::AlternateBase, Qt::transparent); // no zebra striping
    pal.setColor(QPalette::Text, Theme::text);
    pal.setColor(QPalette::PlaceholderText, Theme::textDisabled.lighter(130));
    pal.setColor(QPalette::Button, Theme::button);
    pal.setColor(QPalette::ButtonText, Theme::text);
    pal.setColor(QPalette::BrightText, Theme::detect);
    pal.setColor(QPalette::Highlight, Theme::selected);
    pal.setColor(QPalette::HighlightedText, Theme::text); // selection keeps the text colour
    pal.setColor(QPalette::ToolTipBase, QColor(0x0b, 0x0b, 0x0c));
    pal.setColor(QPalette::ToolTipText, QColor(0xe8, 0xe8, 0xe8));
    pal.setColor(QPalette::Link, Theme::selectedLine);
    pal.setColor(QPalette::Light, Theme::borderHover);
    pal.setColor(QPalette::Midlight, Theme::border);
    pal.setColor(QPalette::Mid, Theme::divider);
    pal.setColor(QPalette::Dark, Theme::sunken);
    pal.setColor(QPalette::Shadow, Qt::black);
    for (const QPalette::ColorRole role : { QPalette::WindowText, QPalette::Text, QPalette::ButtonText }) {
        pal.setColor(QPalette::Disabled, role, Theme::textDisabled);
    }
    return pal;
}

void Style::window(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, Theme::canvas);
}

void Style::card(QPainter& p, const QWidget*, const QRect& rect, const QRect& titleRect, const QString& title) const
{
    panel(p, rect, Theme::panel, Theme::border);
    if (title.isEmpty()) {
        return;
    }
    // section title, then a hairline between it and the content
    p.save();
    p.setFont(Theme::font(10.75));
    p.setPen(Theme::textTitle);
    p.drawText(titleRect.adjusted(12, 2, -12, -4), Qt::AlignLeft | Qt::AlignVCenter, title);
    p.restore();
    p.fillRect(QRect(rect.left() + 12, titleRect.bottom() - 2, rect.width() - 24, 1), Theme::divider);
}

void Style::button(QPainter& p, const QRect& rect, Button kind, const Look& look) const
{
    const QRect r = rect.adjusted(1, 1, -1, -1);
    if (kind == Button::Flat) {
        if (look.enabled && (look.hover || look.pressed || look.checked)) {
            panel(p, r, look.pressed ? Theme::buttonPressed : Theme::selected, QColor());
        }
    } else if (!look.enabled) {
        panel(p, r, kind == Button::Default ? Theme::detectDisabled : QColor(), Theme::borderDisabled);
    } else if (kind == Button::Default) {
        panel(p, r, look.pressed ? Theme::detectPressed : look.hover ? Theme::detectHover : Theme::detectFill, Theme::detect);
    } else if (look.checked) {
        panel(p, r, look.pressed ? Theme::buttonPressed : Theme::selected, Theme::selectedLine);
    } else {
        panel(p, r, look.pressed ? Theme::buttonPressed : look.hover ? Theme::selected : Theme::button, Theme::border);
    }
    if (look.focus) {
        panel(p, r, QColor(), kind == Button::Default ? Theme::detect : Theme::selectedLine);
    }
}

QColor Style::buttonText(Button kind, const Look& look, const QPalette&) const
{
    if (!look.enabled) {
        return Theme::textDisabled;
    }
    return kind == Button::Default ? Theme::detectText : look.checked ? Theme::textSelected : Theme::text;
}

void Style::field(QPainter& p, const QRect& rect, const Look& look) const
{
    const QColor line = !look.enabled ? Theme::borderDisabled : look.focus ? Theme::selectedLine : look.hover ? Theme::borderHover : Theme::border;
    panel(p, rect, Theme::sunken, line);
}

void Style::view(QPainter& p, const QRect& rect, const Look&) const
{
    panel(p, rect, Theme::sunken, Theme::border);
}

void Style::check(QPainter& p, const QRect& rect, Qt::CheckState state, const Look& look) const
{
    QRect box(0, 0, 14, 14);
    box.moveCenter(rect.center());
    panel(p, box, Theme::sunken, !look.enabled ? Theme::borderDisabled : look.hover || look.focus ? Theme::selectedLine : Theme::borderHover);
    const QColor mark = look.enabled ? Theme::detect : Theme::textDisabled;
    const QRectF b(box);
    const auto at = [&](qreal x, qreal y) { return QPointF(b.left() + x * b.width(), b.top() + y * b.height()); };
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(mark, 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    if (state == Qt::Checked) {
        p.drawPolyline(QPolygonF({ at(0.22, 0.52), at(0.42, 0.72), at(0.78, 0.30) }));
    } else if (state == Qt::PartiallyChecked) {
        p.drawLine(at(0.28, 0.5), at(0.72, 0.5));
    }
    p.restore();
}

void Style::radio(QPainter& p, const QRect& rect, bool on, const Look& look) const
{
    QRectF r(0, 0, 14, 14);
    r.moveCenter(QRectF(rect).center());
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(!look.enabled ? Theme::borderDisabled : look.hover || look.focus ? Theme::selectedLine : Theme::borderHover, 1));
    p.setBrush(Theme::sunken);
    p.drawEllipse(r.adjusted(0.5, 0.5, -0.5, -0.5));
    if (on) {
        p.setPen(Qt::NoPen);
        p.setBrush(look.enabled ? Theme::detect : Theme::textDisabled);
        p.drawEllipse(r.center(), 3.5, 3.5);
    }
    p.restore();
}

void Style::arrow(QPainter& p, const QRect& rect, Qt::ArrowType type, const Look& look) const
{
    chevron(p, rect, type, !look.enabled ? Theme::textDisabled : look.hover ? Theme::text : Theme::textMuted);
}

void Style::groove(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation, const Look& look) const
{
    p.fillRect(rect, Theme::borderHover);
    p.fillRect(filled, look.enabled ? Theme::selectedLine : Theme::textDisabled);
}

void Style::handle(QPainter& p, const QRect& rect, Qt::Orientation, const Look& look) const
{
    const QColor face = !look.enabled ? Theme::textDisabled : look.pressed ? Theme::textSelected : look.hover ? Theme::textTitle : Theme::text;
    panel(p, rect, face, look.focus ? Theme::selectedLine : QColor());
}

QSize Style::handleSize(Qt::Orientation) const
{
    return { 10, 10 };
}

void Style::scrollBar(QPainter& p, const QRect&, const QRect& handle, Qt::Orientation orientation, const Look& look) const
{
    const QRect r = orientation == Qt::Horizontal ? handle.adjusted(0, 1, 0, -1) : handle.adjusted(1, 0, -1, 0);
    panel(p, r, look.pressed ? Theme::textSelected : look.hover ? Theme::selectedLine : Theme::borderHover, QColor());
}

void Style::progress(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation orientation) const
{
    // a thin track along the bottom edge, so the value text above it stays clear
    constexpr int Track = 4;
    if (orientation == Qt::Horizontal) {
        const QRect track(rect.left(), rect.bottom() - Track + 1, rect.width(), Track);
        p.fillRect(track, Theme::borderHover);
        p.fillRect(QRect(filled.left(), track.top(), filled.width(), Track), Theme::detect);
    } else {
        const QRect track(rect.left(), rect.top(), Track, rect.height());
        p.fillRect(track, Theme::borderHover);
        p.fillRect(QRect(track.left(), filled.top(), Track, filled.height()), Theme::detect);
    }
}

void Style::tab(QPainter& p, const QRect& rect, bool selected, const Look& look) const
{
    if (!selected) {
        return;
    }
    p.fillRect(rect, Theme::selected);
    p.fillRect(QRect(rect.left(), rect.bottom() - 1, rect.width(), 2), look.enabled ? Theme::selectedLine : Theme::textDisabled);
}

QColor Style::tabText(bool selected, const Look& look, const QPalette&) const
{
    if (!look.enabled) {
        return Theme::textDisabled;
    }
    return selected ? Theme::textSelected : look.hover ? Theme::text : Theme::textTab;
}

void Style::tabPane(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(QRect(rect.left(), rect.top(), rect.width(), 1), Theme::border);
}

void Style::dial(QPainter& p, const QRect& rect, qreal value, const Look& look) const
{
    const QRectF ring = QRectF(rect).adjusted(6, 6, -6, -6);
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Theme::borderHover, 3, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(ring, 225 * 16, -270 * 16);
    p.setPen(QPen(look.enabled ? Theme::selectedLine : Theme::textDisabled, 3, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(ring, 225 * 16, int(-270 * 16 * value));
    const QRectF knob = ring.adjusted(ring.width() * 0.18, ring.height() * 0.18, -ring.width() * 0.18, -ring.height() * 0.18);
    p.setPen(QPen(look.focus || look.hover ? Theme::selectedLine : Theme::border, 1));
    p.setBrush(look.pressed ? Theme::buttonPressed : Theme::button);
    p.drawEllipse(knob);
    p.setPen(QPen(look.enabled ? Theme::text : Theme::textDisabled, 2, Qt::SolidLine, Qt::FlatCap));
    p.drawLine(dialPoint(knob, value, knob.width() * 0.12), dialPoint(knob, value, knob.width() * 0.42));
    p.restore();
}

void Style::display(QPainter& p, const QRect& rect) const
{
    panel(p, rect, Theme::sunken, Theme::border);
}

QColor Style::displayText() const
{
    return Theme::textOverlay;
}

void Style::menuBar(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, Theme::panel);
    p.fillRect(QRect(rect.left(), rect.bottom(), rect.width(), 1), Theme::border);
}

QColor Style::menuBarText(bool active) const
{
    return active ? Theme::textTitle : Theme::text;
}

void Style::menu(QPainter& p, const QRect& rect) const
{
    p.fillRect(rect, Theme::panel);
    p.fillRect(QRect(rect.left(), rect.top(), rect.width(), 1), Theme::border);
    p.fillRect(QRect(rect.left(), rect.bottom(), rect.width(), 1), Theme::border);
    p.fillRect(QRect(rect.left(), rect.top(), 1, rect.height()), Theme::border);
    p.fillRect(QRect(rect.right(), rect.top(), 1, rect.height()), Theme::border);
}

void Style::highlight(QPainter& p, const QRect& rect, bool inBar) const
{
    p.fillRect(inBar ? rect.adjusted(0, 2, 0, -2) : rect.adjusted(2, 0, -2, 0), Theme::selected);
}

void Style::selection(QPainter& p, const QRect& rect, const Look& look) const
{
    p.fillRect(rect, look.checked ? Theme::selected : Theme::rowHover);
}

void Style::header(QPainter& p, const QRect& rect, const Look& look) const
{
    if (look.hover) {
        p.fillRect(rect, Theme::rowHover);
    }
    p.fillRect(QRect(rect.left(), rect.bottom(), rect.width(), 1), Theme::divider);
}

void Style::tooltip(QPainter& p, const QRect& rect) const
{
    p.fillRect(rect, QColor(0x0b, 0x0b, 0x0c)); // the overlay caption chip, made opaque
}

} // namespace flat
