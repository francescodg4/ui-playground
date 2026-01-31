#include "themes/proxmox/Style.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

namespace proxmox {

namespace {

    /// Structure: square, solid fill, 1px border.
    void box(QPainter& p, const QRect& rect, const QColor& fill, const QColor& line)
    {
        if (fill.isValid()) {
            p.fillRect(rect, fill);
        }
        if (line.isValid()) {
            p.save();
            p.setPen(line);
            p.setBrush(Qt::NoBrush);
            p.drawRect(QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5));
            p.restore();
        }
    }

    /// Controls: 2px radius.
    void control(QPainter& p, const QRectF& rect, const QColor& fill, const QColor& line, qreal radius = Theme::radiusControl)
    {
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(line.isValid() ? QPen(line, 1) : QPen(Qt::NoPen));
        p.setBrush(fill.isValid() ? QBrush(fill) : QBrush(Qt::NoBrush));
        p.drawRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
        p.restore();
    }

    /// ExtJS trigger chevron.
    void chevron(QPainter& p, const QRectF& r, Qt::ArrowType type, const QColor& color)
    {
        const auto at = [&](qreal x, qreal y) { return QPointF(r.left() + x * r.width(), r.top() + y * r.height()); };
        QPolygonF line;
        switch (type) {
        case Qt::UpArrow: line = { at(0.1, 0.7), at(0.5, 0.3), at(0.9, 0.7) }; break;
        case Qt::DownArrow: line = { at(0.1, 0.3), at(0.5, 0.7), at(0.9, 0.3) }; break;
        case Qt::LeftArrow: line = { at(0.7, 0.1), at(0.3, 0.5), at(0.7, 0.9) }; break;
        case Qt::RightArrow: line = { at(0.3, 0.1), at(0.7, 0.5), at(0.3, 0.9) }; break;
        default: return;
        }
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(color, 1.3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        p.drawPolyline(line);
        p.restore();
    }

    /// Small filled triangle of a scroll bar's step button.
    void triangle(QPainter& p, const QRectF& r, Qt::ArrowType type, const QColor& color)
    {
        const QPointF c = r.center();
        const qreal s = 3.5;
        QPolygonF tri;
        switch (type) {
        case Qt::UpArrow: tri = { c + QPointF(-s, s / 2), c + QPointF(s, s / 2), c + QPointF(0, -s / 2) }; break;
        case Qt::DownArrow: tri = { c + QPointF(-s, -s / 2), c + QPointF(s, -s / 2), c + QPointF(0, s / 2) }; break;
        case Qt::LeftArrow: tri = { c + QPointF(s / 2, -s), c + QPointF(s / 2, s), c + QPointF(-s / 2, 0) }; break;
        case Qt::RightArrow: tri = { c + QPointF(-s / 2, -s), c + QPointF(-s / 2, s), c + QPointF(s / 2, 0) }; break;
        default: return;
        }
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.drawPolygon(tri);
        p.restore();
    }

} // namespace

Style::Style(bool dark)
    : WidgetStyle([] {
        Metrics m;
        m.control = 24; // toolbar buttons and fields
        m.padding = 10;
        m.frame = 1;
        m.indicator = 15; // a 13px box
        m.arrow = 8;
        m.groove = 4;
        m.scroll = 15;
        m.scrollArrows = true;
        m.tab = 30;
        m.title = 34; // --pve-panel-header
        m.row = 25; // --pve-grid-row
        m.margin = 12;
        m.spacing = 8; // --pve-gutter
        return m;
    }())
    , c(dark ? Theme::dark() : Theme::light())
{
}

QFont Style::font() const
{
    return Theme::font();
}

QPalette Style::standardPalette() const
{
    QPalette pal;
    pal.setColor(QPalette::Window, c.page);
    pal.setColor(QPalette::WindowText, c.text);
    pal.setColor(QPalette::Base, Qt::transparent); // fields and views draw their own surface
    pal.setColor(QPalette::AlternateBase, Qt::transparent); // no alternating rows
    pal.setColor(QPalette::Text, c.text);
    pal.setColor(QPalette::PlaceholderText, c.textPlaceholder);
    pal.setColor(QPalette::Button, c.button);
    pal.setColor(QPalette::ButtonText, c.text);
    pal.setColor(QPalette::BrightText, c.accent);
    pal.setColor(QPalette::Highlight, c.treeSelected);
    pal.setColor(QPalette::HighlightedText, c.text); // selection keeps the text colour
    pal.setColor(QPalette::ToolTipBase, c.tooltip);
    pal.setColor(QPalette::ToolTipText, c.tooltipText);
    pal.setColor(QPalette::Link, c.accent);
    pal.setColor(QPalette::Light, c.surface);
    pal.setColor(QPalette::Midlight, c.surfaceAlt);
    pal.setColor(QPalette::Mid, c.border);
    pal.setColor(QPalette::Dark, c.borderHover);
    pal.setColor(QPalette::Shadow, c.textMuted);
    for (const QPalette::ColorRole role : { QPalette::WindowText, QPalette::Text, QPalette::ButtonText }) {
        pal.setColor(QPalette::Disabled, role, c.textDisabled);
    }
    return pal;
}

void Style::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (element == CE_HeaderLabel) {
        if (const auto* h = qstyleoption_cast<const QStyleOptionHeader*>(option)) {
            QStyleOptionHeader label(*h);
            label.palette.setColor(QPalette::ButtonText, c.textMuted); // grid headers are muted
            WidgetStyle::drawControl(element, &label, painter, widget);
            return;
        }
    }
    WidgetStyle::drawControl(element, option, painter, widget);
}

void Style::window(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, c.page);
}

void Style::card(QPainter& p, const QWidget*, const QRect& rect, const QRect& titleRect, const QString& title) const
{
    // white body; a pale header strip with the blue title
    box(p, rect, c.surface, QColor());
    if (!title.isEmpty()) {
        p.fillRect(QRect(rect.left(), rect.top(), rect.width(), titleRect.height()), c.surfaceAlt);
        p.fillRect(QRect(rect.left(), rect.top() + titleRect.height() - 1, rect.width(), 1), c.border);
        p.save();
        p.setFont(Theme::font(11.25));
        p.setPen(c.accent);
        p.drawText(titleRect.adjusted(10, 0, -10, -1), Qt::AlignLeft | Qt::AlignVCenter, title);
        p.restore();
    }
    box(p, rect, QColor(), c.border);
}

void Style::button(QPainter& p, const QRect& rect, Button kind, const Look& look) const
{
    const QRectF r = QRectF(rect).adjusted(1, 1, -1, -1);
    if (kind == Button::Default) {
        // the primary action (Create VM / Create CT)
        const QColor fill = !look.enabled ? c.accentDisabled : look.pressed ? c.accentPressed : look.hover ? c.accentHover : c.accent;
        control(p, r, fill, QColor());
        if (look.focus) {
            control(p, r.adjusted(1, 1, -1, -1), QColor(), QColor(255, 255, 255, 200), 1);
            control(p, r.adjusted(-1, -1, 1, 1), QColor(), c.accent, Theme::radiusControl + 1);
        }
        return;
    }
    if (kind == Button::Flat) {
        if (look.enabled && (look.hover || look.pressed || look.checked)) {
            control(p, r, look.pressed || look.checked ? c.buttonPressed : c.buttonHover, QColor());
        }
    } else {
        const QColor fill = !look.enabled ? c.buttonDisabled : look.pressed || look.checked ? c.buttonPressed : look.hover ? c.buttonHover : c.button;
        control(p, r, fill, look.checked && look.enabled ? c.borderHover : c.buttonBorder);
        if (look.checked && look.enabled) {
            // pressed in: a shade along the top inner edge
            p.fillRect(QRectF(r.left() + 2, r.top() + 1, r.width() - 4, 1), c.borderHover);
        }
    }
    if (look.focus) {
        control(p, r, QColor(), c.accent);
    }
}

QColor Style::buttonText(Button kind, const Look& look, const QPalette&) const
{
    if (kind == Button::Default) {
        return look.enabled ? QColor(Qt::white) : c.surface;
    }
    return look.enabled ? c.text : c.textDisabled;
}

void Style::field(QPainter& p, const QRect& rect, const Look& look) const
{
    const QColor line = !look.enabled ? c.buttonBorder : look.focus ? c.accent : look.hover ? c.borderHover : c.buttonBorder;
    control(p, QRectF(rect), look.enabled ? c.surface : c.buttonDisabled, line);
}

void Style::view(QPainter& p, const QRect& rect, const Look&) const
{
    box(p, rect, c.surface, c.border);
}

void Style::check(QPainter& p, const QRect& rect, Qt::CheckState state, const Look& look) const
{
    QRectF r(0, 0, 13, 13);
    r.moveCenter(QRectF(rect).center());
    const bool focusFill = look.focus && state != Qt::Unchecked;
    control(p, r, !look.enabled ? c.buttonDisabled : focusFill ? c.accent : c.surface, look.focus ? c.accent : look.hover ? c.borderHover : c.checkBorder, 1);
    const QColor mark = !look.enabled ? c.textDisabled : focusFill ? QColor(Qt::white) : c.check;
    const auto at = [&](qreal x, qreal y) { return QPointF(r.left() + x * r.width(), r.top() + y * r.height()); };
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(mark, 1.8, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    if (state == Qt::Checked) {
        p.drawPolyline(QPolygonF({ at(0.22, 0.52), at(0.42, 0.72), at(0.78, 0.28) }));
    } else if (state == Qt::PartiallyChecked) {
        p.drawLine(at(0.26, 0.5), at(0.74, 0.5));
    }
    p.restore();
}

void Style::radio(QPainter& p, const QRect& rect, bool on, const Look& look) const
{
    QRectF r(0, 0, 13, 13);
    r.moveCenter(QRectF(rect).center());
    const bool focusFill = look.focus && on;
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(look.focus ? c.accent : look.hover ? c.borderHover : c.checkBorder, 1));
    p.setBrush(!look.enabled ? c.buttonDisabled : focusFill ? c.accent : c.surface);
    p.drawEllipse(r.adjusted(0.5, 0.5, -0.5, -0.5));
    if (on) {
        p.setPen(Qt::NoPen);
        p.setBrush(!look.enabled ? c.textDisabled : focusFill ? QColor(Qt::white) : c.check);
        p.drawEllipse(r.center(), 3, 3);
    }
    p.restore();
}

void Style::arrow(QPainter& p, const QRect& rect, Qt::ArrowType type, const Look& look) const
{
    chevron(p, QRectF(rect), type, !look.enabled ? c.textDisabled : look.hover ? c.text : c.textMuted);
}

void Style::groove(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation, const Look& look) const
{
    p.fillRect(rect, c.barTrack);
    p.fillRect(filled, look.enabled ? c.accent : c.textDisabled);
}

void Style::handle(QPainter& p, const QRect& rect, Qt::Orientation, const Look& look) const
{
    // a small square thumb
    const QColor fill = !look.enabled ? c.buttonDisabled : look.pressed ? c.buttonPressed : look.hover ? c.buttonHover : c.button;
    control(p, QRectF(rect), fill, look.focus || look.pressed ? c.accent : c.checkBorder, 1);
}

QSize Style::handleSize(Qt::Orientation orientation) const
{
    return orientation == Qt::Horizontal ? QSize(10, 16) : QSize(16, 10);
}

void Style::scrollBar(QPainter& p, const QRect& groove, const QRect& handle, Qt::Orientation orientation, const Look& look) const
{
    // track, a step button with a small triangle at each end, and a bordered thumb
    p.fillRect(groove, c.scrollTrack);
    const bool horizontal = orientation == Qt::Horizontal;
    const int t = horizontal ? groove.height() : groove.width();
    const QRect first = horizontal ? QRect(groove.left(), groove.top(), t, t) : QRect(groove.left(), groove.top(), t, t);
    const QRect last = horizontal ? QRect(groove.right() - t + 1, groove.top(), t, t) : QRect(groove.left(), groove.bottom() - t + 1, t, t);
    triangle(p, QRectF(first), horizontal ? Qt::LeftArrow : Qt::UpArrow, c.scrollArrow);
    triangle(p, QRectF(last), horizontal ? Qt::RightArrow : Qt::DownArrow, c.scrollArrow);
    const QRect thumb = horizontal ? handle.adjusted(0, 3, 0, -3) : handle.adjusted(3, 0, -3, 0);
    const QColor fill = look.pressed || look.hover ? c.scrollThumbHover : c.scrollThumb;
    box(p, thumb, fill, fill.darker(112));
}

void Style::progress(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation orientation) const
{
    // a thin usage bar; the label sits above it, never inside
    constexpr int Bar = 4;
    if (orientation == Qt::Horizontal) {
        const QRect track(rect.left(), rect.bottom() - Bar + 1, rect.width(), Bar);
        p.fillRect(track, c.barTrack);
        p.fillRect(QRect(filled.left(), track.top(), filled.width(), Bar), c.barFill);
    } else {
        const QRect track(rect.left(), rect.top(), Bar, rect.height());
        p.fillRect(track, c.barTrack);
        p.fillRect(QRect(track.left(), filled.top(), Bar, filled.height()), c.barFill);
    }
}

void Style::tab(QPainter& p, const QRect& rect, bool selected, const Look& look) const
{
    // the Tasks / Cluster log tabs: the active one is solid blue, the others plain text
    const QRectF r = QRectF(rect).adjusted(2, 3, -2, -3);
    if (selected) {
        control(p, r, look.enabled ? c.accent : c.accentDisabled, QColor());
    }
    if (look.focus) {
        control(p, r.adjusted(-1, -1, 1, 1), QColor(), c.accent, Theme::radiusControl + 1);
    }
}

QColor Style::tabText(bool selected, const Look& look, const QPalette&) const
{
    if (!look.enabled) {
        return c.textDisabled;
    }
    return selected ? QColor(Qt::white) : look.hover ? c.accent : c.text;
}

void Style::tabPane(QPainter&, const QWidget*, const QRect&) const
{
}

void Style::dial(QPainter& p, const QRect& rect, qreal value, const Look& look) const
{
    // Proxmox has no dial: a plain ring with the value in blue, like its usage bars
    const QRectF ring = QRectF(rect).adjusted(6, 6, -6, -6);
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(c.barTrack, 4, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(ring, 225 * 16, -270 * 16);
    p.setPen(QPen(look.enabled ? c.accent : c.textDisabled, 4, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(ring, 225 * 16, int(-270 * 16 * value));
    const QRectF knob = ring.adjusted(ring.width() * 0.2, ring.height() * 0.2, -ring.width() * 0.2, -ring.height() * 0.2);
    p.setPen(QPen(look.focus ? c.accent : look.hover ? c.borderHover : c.buttonBorder, 1));
    p.setBrush(look.pressed ? c.buttonPressed : c.button);
    p.drawEllipse(knob);
    p.setPen(QPen(look.enabled ? c.text : c.textDisabled, 2, Qt::SolidLine, Qt::FlatCap));
    p.drawLine(dialPoint(knob, value, knob.width() * 0.12), dialPoint(knob, value, knob.width() * 0.42));
    p.restore();
}

void Style::display(QPainter& p, const QRect& rect) const
{
    // plain text on the panel surface, no digital glass
    box(p, rect, c.surface, c.border);
}

QColor Style::displayText() const
{
    return c.text;
}

void Style::menuBar(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, c.header); // the header bar carries the menus
}

QColor Style::menuBarText(bool) const
{
    return c.text;
}

void Style::menu(QPainter& p, const QRect& rect) const
{
    box(p, rect, c.surface, c.border);
}

void Style::highlight(QPainter& p, const QRect& rect, bool inBar) const
{
    if (inBar) {
        control(p, QRectF(rect).adjusted(0, 3, 0, -3), c.buttonHover, c.buttonBorder);
    } else {
        p.fillRect(rect.adjusted(1, 0, -1, 0), c.menuHover);
    }
}

void Style::selection(QPainter& p, const QRect& rect, const Look& look) const
{
    if (look.checked) {
        // the resource tree's selection: pale blue with a 1px blue outline
        box(p, rect, c.treeSelected, c.accent);
    } else {
        p.fillRect(rect, c.hover);
    }
}

void Style::header(QPainter& p, const QRect& rect, const Look& look) const
{
    p.fillRect(rect, look.hover ? c.menuHover : c.surfaceAlt);
    p.fillRect(QRect(rect.left(), rect.bottom(), rect.width(), 1), c.border);
    p.fillRect(QRect(rect.right(), rect.top(), 1, rect.height()), c.gridLine);
}

void Style::tooltip(QPainter& p, const QRect& rect) const
{
    box(p, rect, c.tooltip, c.tooltipBorder);
}

} // namespace proxmox
