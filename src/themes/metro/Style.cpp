#include "themes/metro/Style.hpp"

#include "themes/metro/Theme.hpp"

#include <QPainter>
#include <QPainterPath>

namespace metro {

namespace {

    QColor alpha(QColor c, int a)
    {
        c.setAlpha(a);
        return c;
    }

    /// Square border of @p width pixels inside @p r.
    void border(QPainter& p, const QRect& r, const QColor& color, int width = 2)
    {
        p.fillRect(QRect(r.left(), r.top(), r.width(), width), color);
        p.fillRect(QRect(r.left(), r.bottom() - width + 1, r.width(), width), color);
        p.fillRect(QRect(r.left(), r.top() + width, width, r.height() - 2 * width), color);
        p.fillRect(QRect(r.right() - width + 1, r.top() + width, width, r.height() - 2 * width), color);
    }

    /// Keyboard focus: a dotted rectangle just inside the control, as in Windows 8.
    void focusRect(QPainter& p, const QRect& r, const QColor& color)
    {
        p.save();
        p.setPen(QPen(color, 1, Qt::DotLine));
        p.setBrush(Qt::NoBrush);
        p.drawRect(QRectF(r).adjusted(4.5, 4.5, -4.5, -4.5));
        p.restore();
    }

    /// Line chevron, like the glyphs of Segoe UI Symbol.
    void chevron(QPainter& p, const QRect& rect, Qt::ArrowType type, const QColor& color)
    {
        const QRectF r(rect);
        const auto at = [&](qreal x, qreal y) { return QPointF(r.left() + x * r.width(), r.top() + y * r.height()); };
        QPolygonF line;
        switch (type) {
        case Qt::UpArrow: line = { at(0, 0.72), at(0.5, 0.25), at(1, 0.72) }; break;
        case Qt::DownArrow: line = { at(0, 0.28), at(0.5, 0.75), at(1, 0.28) }; break;
        case Qt::LeftArrow: line = { at(0.72, 0), at(0.25, 0.5), at(0.72, 1) }; break;
        case Qt::RightArrow: line = { at(0.28, 0), at(0.75, 0.5), at(0.28, 1) }; break;
        default: return;
        }
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(color, 1.6, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        p.drawPolyline(line);
        p.restore();
    }

    const QColor disabledFill { 0x5a, 0x5a, 0x5a };
    const QColor disabledText { 0x6d, 0x6d, 0x6d };

} // namespace

Style::Style()
    : WidgetStyle([] {
        Metrics m;
        m.control = 32;
        m.padding = 16;
        m.frame = 2;
        m.indicator = 20;
        m.arrow = 10;
        m.groove = 8;
        m.scroll = 10;
        m.tab = 40;
        m.title = 52;
        m.row = 30;
        m.margin = 16;
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
    pal.setColor(QPalette::WindowText, Theme::white);
    pal.setColor(QPalette::Base, Qt::transparent); // fields and views draw their white surface
    pal.setColor(QPalette::AlternateBase, Theme::light);
    pal.setColor(QPalette::Text, Theme::text);
    pal.setColor(QPalette::PlaceholderText, QColor(0x7a, 0x7a, 0x7a));
    pal.setColor(QPalette::Button, Theme::canvas);
    pal.setColor(QPalette::ButtonText, Theme::white);
    pal.setColor(QPalette::BrightText, Theme::orange);
    pal.setColor(QPalette::Highlight, Theme::accent);
    pal.setColor(QPalette::HighlightedText, Theme::white);
    pal.setColor(QPalette::ToolTipBase, Theme::white);
    pal.setColor(QPalette::ToolTipText, Theme::text);
    pal.setColor(QPalette::Link, Theme::blue);
    pal.setColor(QPalette::Light, Theme::white);
    pal.setColor(QPalette::Midlight, Theme::light);
    pal.setColor(QPalette::Mid, Theme::dim);
    pal.setColor(QPalette::Dark, Theme::track);
    pal.setColor(QPalette::Shadow, Qt::black);
    pal.setColor(QPalette::Disabled, QPalette::WindowText, disabledText);
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, disabledText);
    pal.setColor(QPalette::Disabled, QPalette::Text, QColor(0x9a, 0x9a, 0x9a));
    return pal;
}

void Style::window(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, Theme::canvas);
}

void Style::card(QPainter& p, const QWidget*, const QRect&, const QRect& titleRect, const QString& title) const
{
    // content is chrome: a group is only its title, large and light
    if (title.isEmpty()) {
        return;
    }
    p.save();
    p.setFont(Theme::font(18, QFont::Light));
    p.setPen(Theme::white);
    p.drawText(titleRect.adjusted(16, 0, -16, -4), Qt::AlignLeft | Qt::AlignVCenter, title);
    p.restore();
}

void Style::button(QPainter& p, const QRect& rect, Button kind, const Look& look) const
{
    const QRect r = rect.adjusted(1, 1, -1, -1);
    if (!look.enabled) {
        if (kind != Button::Flat) {
            border(p, r, disabledFill);
        }
        return;
    }
    if (look.pressed) {
        p.fillRect(r, Theme::white); // pressed buttons invert
    } else if (kind == Button::Flat) {
        if (look.hover || look.checked) {
            p.fillRect(r, alpha(Theme::white, look.checked ? 50 : 28));
        }
    } else if (kind == Button::Default || look.checked) {
        p.fillRect(r, look.hover ? Theme::accent.lighter(115) : Theme::accent);
        border(p, r, look.hover ? Theme::accent.lighter(115) : Theme::accent);
    } else {
        if (look.hover) {
            p.fillRect(r, alpha(Theme::white, 28));
        }
        border(p, r, Theme::white);
    }
    if (look.focus) {
        focusRect(p, r, look.pressed ? Theme::text : Theme::white);
    }
}

QColor Style::buttonText(Button, const Look& look, const QPalette&) const
{
    if (!look.enabled) {
        return disabledText;
    }
    return look.pressed ? Theme::text : Theme::white;
}

void Style::field(QPainter& p, const QRect& rect, const Look& look) const
{
    if (!look.enabled) {
        p.fillRect(rect, disabledFill);
        return;
    }
    p.fillRect(rect, look.hover || look.focus ? Theme::white : Theme::light);
    if (look.focus) {
        border(p, rect, Theme::accent);
    }
}

void Style::view(QPainter& p, const QRect& rect, const Look& look) const
{
    p.fillRect(rect, look.enabled ? Theme::white : QColor(0xb4, 0xb4, 0xb4));
}

void Style::check(QPainter& p, const QRect& rect, Qt::CheckState state, const Look& look) const
{
    const QRect r = rect.adjusted(1, 1, -1, -1);
    p.fillRect(r, !look.enabled ? disabledFill : look.pressed ? Theme::hover : Theme::white);
    border(p, r, look.hover ? Theme::accent : Theme::dim);
    const QColor ink = look.enabled ? Theme::text : QColor(0x9a, 0x9a, 0x9a);
    if (state == Qt::Checked) {
        const QRectF b(r);
        const auto at = [&](qreal x, qreal y) { return QPointF(b.left() + x * b.width(), b.top() + y * b.height()); };
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(ink, 2.2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        p.drawPolyline(QPolygonF({ at(0.24, 0.52), at(0.42, 0.70), at(0.76, 0.30) }));
        p.restore();
    } else if (state == Qt::PartiallyChecked) {
        QRect square(0, 0, r.width() / 2, r.height() / 2);
        square.moveCenter(r.center());
        p.fillRect(square, ink);
    }
}

void Style::radio(QPainter& p, const QRect& rect, bool on, const Look& look) const
{
    const QRectF r = QRectF(rect).adjusted(2, 2, -2, -2);
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(look.hover ? Theme::accent : Theme::dim, 2));
    p.setBrush(!look.enabled ? disabledFill : look.pressed ? Theme::hover : Theme::white);
    p.drawEllipse(r);
    if (on) {
        p.setPen(Qt::NoPen);
        p.setBrush(look.enabled ? Theme::text : QColor(0x9a, 0x9a, 0x9a));
        p.drawEllipse(r.center(), r.width() * 0.24, r.width() * 0.24);
    }
    p.restore();
}

void Style::arrow(QPainter& p, const QRect& rect, Qt::ArrowType type, const Look& look) const
{
    chevron(p, rect, type, look.enabled ? (look.pressed ? Theme::text : Theme::white) : disabledText);
}

void Style::spinButton(QPainter& p, const QRect& rect, Qt::ArrowType type, const Look& look) const
{
    // on the white field: a grey cell when hovered, a dark chevron
    if (look.pressed) {
        p.fillRect(rect, Theme::accent);
    } else if (look.hover) {
        p.fillRect(rect, Theme::hover);
    }
    QRect a(0, 0, 9, 9);
    a.moveCenter(rect.center());
    chevron(p, a, type, !look.enabled ? QColor(0xb0, 0xb0, 0xb0) : look.pressed ? Theme::white : Theme::text);
}

void Style::groove(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation, const Look& look) const
{
    p.fillRect(rect, Theme::track);
    p.fillRect(filled, look.enabled ? Theme::accent : disabledText);
}

void Style::handle(QPainter& p, const QRect& rect, Qt::Orientation, const Look& look) const
{
    p.fillRect(rect, !look.enabled ? disabledFill : look.pressed || look.hover ? Theme::white : Theme::light);
    if (look.focus) {
        border(p, rect, Theme::accent, 1);
    }
}

QSize Style::handleSize(Qt::Orientation orientation) const
{
    return orientation == Qt::Horizontal ? QSize(10, 26) : QSize(26, 10);
}

void Style::scrollBar(QPainter& p, const QRect&, const QRect& handle, Qt::Orientation orientation, const Look& look) const
{
    const QRect r = orientation == Qt::Horizontal ? handle.adjusted(0, 2, 0, -2) : handle.adjusted(2, 0, -2, 0);
    p.fillRect(r, look.pressed ? QColor(0x3a, 0x3a, 0x3a) : look.hover ? QColor(0x6a, 0x6a, 0x6a) : QColor(0x9a, 0x9a, 0x9a));
}

void Style::progress(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation) const
{
    // a light track keeps the dark label readable; the fill is the accent
    p.fillRect(rect, QColor(0xd6, 0xd6, 0xd6));
    p.fillRect(filled, Theme::accent);
}

void Style::tab(QPainter& p, const QRect& rect, bool selected, const Look& look) const
{
    // tabs are text headers; the selected one carries an accent bar
    const QRect bar(rect.left() + 8, rect.bottom() - 3, rect.width() - 16, 3);
    if (selected) {
        p.fillRect(bar, look.enabled ? Theme::accent : disabledText);
    } else if (look.hover) {
        p.fillRect(bar, alpha(Theme::white, 70));
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
    p.setPen(QPen(Theme::track, 8, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(ring, 225 * 16, -270 * 16);
    p.setPen(QPen(look.enabled ? Theme::accent : disabledText, 8, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(ring, 225 * 16, int(-270 * 16 * value));
    const QRectF knob = ring.adjusted(ring.width() * 0.2, ring.height() * 0.2, -ring.width() * 0.2, -ring.height() * 0.2);
    p.setPen(Qt::NoPen);
    p.setBrush(look.pressed ? Theme::white : look.hover ? QColor(0x3a, 0x3a, 0x3a) : QColor(0x2b, 0x2b, 0x2b));
    p.drawEllipse(knob);
    p.setPen(QPen(look.pressed ? Theme::text : look.enabled ? Theme::white : disabledText, 3, Qt::SolidLine, Qt::FlatCap));
    p.drawLine(dialPoint(knob, value, knob.width() * 0.1), dialPoint(knob, value, knob.width() * 0.42));
    p.restore();
}

void Style::display(QPainter& p, const QRect& rect) const
{
    p.fillRect(rect, Theme::purple); // a live tile
}

QColor Style::displayText() const
{
    return Theme::white;
}

void Style::menuBar(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, Theme::charms);
}

QColor Style::menuBarText(bool) const
{
    return Theme::white;
}

void Style::menu(QPainter& p, const QRect& rect) const
{
    p.fillRect(rect, Theme::white);
    border(p, rect, Theme::dim, 1);
}

void Style::highlight(QPainter& p, const QRect& rect, bool inBar) const
{
    p.fillRect(inBar ? rect : rect.adjusted(1, 0, -1, 0), inBar ? alpha(Theme::white, 40) : Theme::accent);
}

void Style::selection(QPainter& p, const QRect& rect, const Look& look) const
{
    p.fillRect(rect, look.checked ? Theme::accent : Theme::hover);
}

void Style::header(QPainter& p, const QRect& rect, const Look& look) const
{
    p.fillRect(rect, look.hover ? QColor(0x3d, 0x3d, 0x3d) : QColor(0x33, 0x33, 0x33));
    p.fillRect(QRect(rect.right(), rect.top() + 6, 1, rect.height() - 12), QColor(0x55, 0x55, 0x55));
}

void Style::tooltip(QPainter& p, const QRect& rect) const
{
    menu(p, rect);
}

} // namespace metro
