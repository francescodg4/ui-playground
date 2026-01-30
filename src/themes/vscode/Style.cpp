#include "themes/vscode/Style.hpp"

#include "themes/vscode/Theme.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QTextEdit>

namespace vscode {

namespace {

    /// Chrome surface: square, solid fill, 1px border.
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

    /// Widget surface: lightly rounded (2-6px).
    void rounded(QPainter& p, const QRectF& rect, qreal radius, const QColor& fill, const QColor& line = QColor())
    {
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(line.isValid() ? QPen(line, 1) : QPen(Qt::NoPen));
        p.setBrush(fill.isValid() ? QBrush(fill) : QBrush(Qt::NoBrush));
        p.drawRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
        p.restore();
    }

    /// Codicon-style line chevron.
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

    /// The always-visible focus ring: 1px --focus.
    void focusRing(QPainter& p, const QRectF& rect, qreal radius)
    {
        rounded(p, rect, radius, QColor(), Theme::focus);
    }

    constexpr qreal DisabledOpacity = 0.4;

} // namespace

Style::Style()
    : WidgetStyle([] {
        Metrics m;
        m.control = 26;
        m.padding = 12;
        m.frame = 1;
        m.indicator = 18;
        m.arrow = 10;
        m.groove = 4;
        m.scroll = 10;
        m.tab = 35; // --tab-height
        m.title = 30;
        m.row = 22; // --row-height
        m.margin = 12;
        m.spacing = 8; // 4px grid
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
    pal.setColor(QPalette::Window, Theme::editor);
    pal.setColor(QPalette::WindowText, Theme::fg);
    pal.setColor(QPalette::Base, Qt::transparent); // fields and views draw their own surface
    pal.setColor(QPalette::AlternateBase, Qt::transparent); // no zebra striping
    pal.setColor(QPalette::Text, Theme::fg);
    pal.setColor(QPalette::PlaceholderText, Theme::fgMuted);
    pal.setColor(QPalette::Button, Theme::buttonSecondary);
    pal.setColor(QPalette::ButtonText, Theme::fgBright);
    pal.setColor(QPalette::BrightText, Theme::fgBright);
    pal.setColor(QPalette::Highlight, Theme::selected);
    pal.setColor(QPalette::HighlightedText, Theme::fgBright);
    pal.setColor(QPalette::ToolTipBase, Theme::sidebar);
    pal.setColor(QPalette::ToolTipText, Theme::fg);
    pal.setColor(QPalette::Link, Theme::link);
    pal.setColor(QPalette::Light, Theme::widgetBorder);
    pal.setColor(QPalette::Midlight, Theme::border);
    pal.setColor(QPalette::Mid, Theme::border);
    pal.setColor(QPalette::Dark, Theme::editor);
    pal.setColor(QPalette::Shadow, Qt::black);
    for (const QPalette::ColorRole role : { QPalette::WindowText, QPalette::Text, QPalette::ButtonText }) {
        pal.setColor(QPalette::Disabled, role, Theme::fgDisabled);
    }
    return pal;
}

void Style::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (element == PE_Frame && qobject_cast<const QTextEdit*>(widget)) {
        // text reads on the editor canvas
        box(*painter, option->rect, Theme::editor, Theme::border);
        return;
    }
    WidgetStyle::drawPrimitive(element, option, painter, widget);
}

void Style::window(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, Theme::editor);
}

void Style::card(QPainter& p, const QWidget*, const QRect& rect, const QRect& titleRect, const QString& title) const
{
    box(p, rect, Theme::sidebar, Theme::border);
    if (title.isEmpty()) {
        return;
    }
    // a sidebar section header: chevron, then 11px bold uppercase
    chevron(p, QRectF(titleRect.left() + 10, titleRect.center().y() - 4, 9, 9), Qt::DownArrow, Theme::fg);
    QFont f = Theme::font(8.25, QFont::Bold);
    f.setCapitalization(QFont::AllUppercase);
    p.save();
    p.setFont(f);
    p.setPen(Theme::fg);
    p.drawText(titleRect.adjusted(26, 0, -12, 0), Qt::AlignLeft | Qt::AlignVCenter, title);
    p.restore();
}

void Style::button(QPainter& p, const QRect& rect, Button kind, const Look& look) const
{
    const QRectF r = QRectF(rect).adjusted(2, 2, -2, -2); // room for the offset focus ring
    p.save();
    if (!look.enabled) {
        p.setOpacity(DisabledOpacity);
    }
    if (kind == Button::Flat) {
        // toolbar action: no surface until hovered
        if (look.enabled && (look.hover || look.pressed || look.checked)) {
            rounded(p, r, 5, QColor(90, 93, 94, look.pressed ? 110 : 80));
        }
    } else {
        const bool primary = kind == Button::Default || look.checked;
        const bool lit = look.enabled && (look.hover || look.pressed);
        rounded(p, r, Theme::radiusSmall, primary ? (lit ? Theme::buttonHover : Theme::button) : (lit ? Theme::buttonSecondaryHover : Theme::buttonSecondary));
    }
    p.restore();
    if (look.focus) {
        focusRing(p, QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5), Theme::radiusSmall + 1);
    }
}

QColor Style::buttonText(Button, const Look& look, const QPalette&) const
{
    QColor text = Theme::fgBright;
    if (!look.enabled) {
        text.setAlphaF(DisabledOpacity);
    }
    return text;
}

void Style::field(QPainter& p, const QRect& rect, const Look& look) const
{
    p.save();
    if (!look.enabled) {
        p.setOpacity(DisabledOpacity);
    }
    rounded(p, QRectF(rect), Theme::radiusSmall, Theme::input, look.focus ? Theme::focus : QColor());
    p.restore();
}

void Style::view(QPainter& p, const QRect& rect, const Look&) const
{
    box(p, rect, Theme::sidebar, Theme::border);
}

void Style::check(QPainter& p, const QRect& rect, Qt::CheckState state, const Look& look) const
{
    QRectF r(0, 0, 18, 18);
    r.moveCenter(QRectF(rect).center());
    p.save();
    if (!look.enabled) {
        p.setOpacity(DisabledOpacity);
    }
    rounded(p, r, Theme::radiusCheck, Theme::input, Theme::checkBorder);
    const auto at = [&](qreal x, qreal y) { return QPointF(r.left() + x * r.width(), r.top() + y * r.height()); };
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Theme::fgBright, 1.6, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    if (state == Qt::Checked) {
        p.drawPolyline(QPolygonF({ at(0.24, 0.52), at(0.42, 0.70), at(0.77, 0.32) }));
    } else if (state == Qt::PartiallyChecked) {
        p.drawLine(at(0.28, 0.5), at(0.72, 0.5));
    }
    p.restore();
    if (look.focus) {
        focusRing(p, r.adjusted(-2, -2, 2, 2), Theme::radiusCheck + 2);
    }
}

void Style::radio(QPainter& p, const QRect& rect, bool on, const Look& look) const
{
    QRectF r(0, 0, 18, 18);
    r.moveCenter(QRectF(rect).center());
    p.save();
    if (!look.enabled) {
        p.setOpacity(DisabledOpacity);
    }
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Theme::checkBorder, 1));
    p.setBrush(Theme::input);
    p.drawEllipse(r.adjusted(0.5, 0.5, -0.5, -0.5));
    if (on) {
        p.setPen(Qt::NoPen);
        p.setBrush(Theme::fgBright);
        p.drawEllipse(r.center(), 4, 4);
    }
    p.restore();
    if (look.focus) {
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(Theme::focus, 1));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(r.adjusted(-1.5, -1.5, 1.5, 1.5));
        p.restore();
    }
}

void Style::arrow(QPainter& p, const QRect& rect, Qt::ArrowType type, const Look& look) const
{
    chevron(p, QRectF(rect), type, !look.enabled ? Theme::fgDisabled : look.hover ? Theme::fgBright : Theme::fg);
}

void Style::groove(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation, const Look& look) const
{
    p.fillRect(rect, Theme::border);
    p.fillRect(filled, look.enabled ? Theme::accent : Theme::fgDisabled);
}

void Style::handle(QPainter& p, const QRect& rect, Qt::Orientation, const Look& look) const
{
    const QRectF r = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(look.enabled ? Theme::fgBright : Theme::fgDisabled);
    p.drawEllipse(r);
    if (look.focus) {
        p.setPen(QPen(Theme::focus, 1));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(r.adjusted(-1.5, -1.5, 1.5, 1.5));
    }
    p.restore();
}

QSize Style::handleSize(Qt::Orientation) const
{
    return { 12, 12 };
}

void Style::scrollBar(QPainter& p, const QRect&, const QRect& handle, Qt::Orientation, const Look& look) const
{
    // translucent square thumb, no track and no arrows
    p.fillRect(handle, look.pressed ? QColor(0xbf, 0xbf, 0xbf, 0x66) : look.hover ? QColor(0x64, 0x64, 0x64, 0xb3) : QColor(0x79, 0x79, 0x79, 0x66));
}

void Style::progress(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation orientation) const
{
    // the 2px bar that runs along the top of a view
    if (orientation == Qt::Horizontal) {
        p.fillRect(QRect(filled.left(), rect.top(), filled.width(), 2), Theme::progress);
    } else {
        p.fillRect(QRect(rect.left(), filled.top(), 2, filled.height()), Theme::progress);
    }
}

void Style::tab(QPainter& p, const QRect& rect, bool selected, const Look& look) const
{
    // tabs are slightly rounded at the top and square where they meet the editor
    const QRectF r = QRectF(rect).adjusted(0, 0, -1, 0); // 1px gap between tabs
    QPainterPath shape;
    shape.moveTo(r.bottomLeft());
    shape.lineTo(r.left(), r.top() + Theme::radiusTab);
    shape.quadTo(r.topLeft(), QPointF(r.left() + Theme::radiusTab, r.top()));
    shape.lineTo(r.right() - Theme::radiusTab, r.top());
    shape.quadTo(r.topRight(), QPointF(r.right(), r.top() + Theme::radiusTab));
    shape.lineTo(r.bottomRight());
    shape.closeSubpath();
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.fillPath(shape, selected ? Theme::editor : Theme::tabInactive);
    if (look.focus) {
        p.strokePath(shape.translated(0.5, 0.5), QPen(Theme::focus, 1));
    }
    p.restore();
}

QColor Style::tabText(bool selected, const Look& look, const QPalette&) const
{
    if (!look.enabled) {
        return Theme::fgDisabled;
    }
    return selected || look.hover ? Theme::fgBright : Theme::fgTab;
}

void Style::tabPane(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, Theme::editor);
}

void Style::dial(QPainter& p, const QRect& rect, qreal value, const Look& look) const
{
    const QRectF ring = QRectF(rect).adjusted(6, 6, -6, -6);
    p.save();
    if (!look.enabled) {
        p.setOpacity(DisabledOpacity);
    }
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Theme::border, 4, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(ring, 225 * 16, -270 * 16);
    p.setPen(QPen(Theme::accent, 4, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(ring, 225 * 16, int(-270 * 16 * value));
    const QRectF knob = ring.adjusted(ring.width() * 0.2, ring.height() * 0.2, -ring.width() * 0.2, -ring.height() * 0.2);
    p.setPen(look.focus ? QPen(Theme::focus, 1) : QPen(Qt::NoPen));
    p.setBrush(look.hover || look.pressed ? Theme::buttonSecondaryHover : Theme::buttonSecondary);
    p.drawEllipse(knob);
    p.setPen(QPen(Theme::fgBright, 2, Qt::SolidLine, Qt::FlatCap));
    p.drawLine(dialPoint(knob, value, knob.width() * 0.12), dialPoint(knob, value, knob.width() * 0.42));
    p.restore();
}

void Style::display(QPainter& p, const QRect& rect) const
{
    // the status bar's accent block: flush, square, white text
    p.fillRect(rect, Theme::accent);
}

QColor Style::displayText() const
{
    return Theme::fgBright;
}

void Style::menuBar(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, Theme::titlebar); // the title bar carries the menu
}

QColor Style::menuBarText(bool active) const
{
    return active ? Theme::fgBright : Theme::fg;
}

void Style::menu(QPainter& p, const QRect& rect) const
{
    box(p, rect, Theme::sidebar, Theme::widgetBorder);
}

void Style::highlight(QPainter& p, const QRect& rect, bool inBar) const
{
    if (inBar) {
        rounded(p, QRectF(rect).adjusted(0, 3, 0, -3), 4, QColor(255, 255, 255, 26));
    } else {
        p.fillRect(rect.adjusted(4, 0, -4, 0), Theme::selected);
    }
}

void Style::selection(QPainter& p, const QRect& rect, const Look& look) const
{
    p.fillRect(rect, look.checked ? Theme::selected : Theme::hover);
}

void Style::header(QPainter& p, const QRect& rect, const Look& look) const
{
    p.fillRect(rect, look.hover ? Theme::hover : Theme::sidebar);
    p.fillRect(QRect(rect.left(), rect.bottom(), rect.width(), 1), Theme::border);
}

void Style::tooltip(QPainter& p, const QRect& rect) const
{
    box(p, rect, Theme::sidebar, Theme::widgetBorder);
}

} // namespace vscode
