#include "themes/claudecode/Style.hpp"

#include "themes/claudecode/Theme.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QTextDocument>
#include <QTextEdit>

namespace claudecode {

namespace {

    /// Chrome surface: square, solid fill, 1px hairline.
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

    /// Paper (or accent) surface: rounded.
    void rounded(QPainter& p, const QRectF& rect, qreal radius, const QColor& fill, const QColor& line = QColor())
    {
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(line.isValid() ? QPen(line, 1) : QPen(Qt::NoPen));
        p.setBrush(fill.isValid() ? QBrush(fill) : QBrush(Qt::NoBrush));
        p.drawRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
        p.restore();
    }

    void chevron(QPainter& p, const QRectF& r, Qt::ArrowType type, const QColor& color, qreal width = 1.4)
    {
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
        p.setPen(QPen(color, width, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        p.drawPolyline(line);
        p.restore();
    }

    QColor alpha(QColor c, int a)
    {
        c.setAlpha(a);
        return c;
    }

    const QColor paperSelection { 0xf1, 0xd9, 0xcd }; ///< text selection on paper

} // namespace

Style::Style()
    : WidgetStyle([] {
        Metrics m;
        m.control = 28;
        m.padding = 12;
        m.frame = 1;
        m.indicator = 16;
        m.arrow = 8;
        m.groove = 2;
        m.scroll = 8;
        m.tab = 32; // --tab-height
        m.title = 32;
        m.row = 22; // --row-height
        m.margin = 12;
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
    pal.setColor(QPalette::Window, Theme::chrome0);
    pal.setColor(QPalette::WindowText, Theme::chromeText);
    pal.setColor(QPalette::Base, Qt::transparent); // fields and views draw their own surface
    pal.setColor(QPalette::AlternateBase, Qt::transparent);
    pal.setColor(QPalette::Text, Theme::chromeText);
    pal.setColor(QPalette::PlaceholderText, Theme::chromeTextMuted);
    pal.setColor(QPalette::Button, Theme::chrome1);
    pal.setColor(QPalette::ButtonText, Theme::chromeText);
    pal.setColor(QPalette::BrightText, Theme::accent);
    pal.setColor(QPalette::Highlight, Theme::chromeSelected);
    pal.setColor(QPalette::HighlightedText, Theme::chromeTextBright);
    pal.setColor(QPalette::ToolTipBase, Theme::chrome0);
    pal.setColor(QPalette::ToolTipText, Theme::chromeText);
    pal.setColor(QPalette::Link, Theme::accent);
    pal.setColor(QPalette::Light, Theme::chromeBorderHover);
    pal.setColor(QPalette::Midlight, Theme::chromeBorder);
    pal.setColor(QPalette::Mid, Theme::chromeBorder);
    pal.setColor(QPalette::Dark, Theme::chrome0);
    pal.setColor(QPalette::Shadow, Qt::black);
    for (const QPalette::ColorRole role : { QPalette::WindowText, QPalette::Text, QPalette::ButtonText }) {
        pal.setColor(QPalette::Disabled, role, Theme::chromeTextDisabled);
    }
    return pal;
}

void Style::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    if (element == PE_Frame && qobject_cast<const QTextEdit*>(widget)) {
        // prose: a rounded cream paper card
        rounded(*painter, QRectF(option->rect), Theme::radiusCard, Theme::paper, Theme::paperBorder);
        return;
    }
    WidgetStyle::drawPrimitive(element, option, painter, widget);
}

void Style::polish(QWidget* widget)
{
    WidgetStyle::polish(widget);
    if (auto* text = qobject_cast<QTextEdit*>(widget)) {
        QPalette pal = text->palette();
        pal.setColor(QPalette::Text, Theme::paperText);
        pal.setColor(QPalette::PlaceholderText, Theme::paperTextMuted);
        pal.setColor(QPalette::Highlight, paperSelection);
        pal.setColor(QPalette::HighlightedText, Theme::paperText);
        text->setPalette(pal);
        text->document()->setDocumentMargin(16); // paper breathes
    }
}

void Style::unpolish(QWidget* widget)
{
    if (auto* text = qobject_cast<QTextEdit*>(widget)) {
        text->setPalette(QPalette());
        text->document()->setDocumentMargin(4); // Qt default
    }
    WidgetStyle::unpolish(widget);
}

void Style::window(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, Theme::chrome0);
}

void Style::card(QPainter& p, const QWidget*, const QRect& rect, const QRect& titleRect, const QString& title) const
{
    box(p, rect, Theme::chrome1, Theme::chromeBorder);
    if (title.isEmpty()) {
        return;
    }
    // a sidebar section header: disclosure chevron, then a small uppercase muted label
    chevron(p, QRectF(titleRect.left() + 12, titleRect.center().y() - 3, 8, 8), Qt::DownArrow, Theme::chromeTextMuted, 1.3);
    QFont f = Theme::font(8.25, QFont::DemiBold);
    f.setCapitalization(QFont::AllUppercase);
    f.setLetterSpacing(QFont::PercentageSpacing, 106);
    p.save();
    p.setFont(f);
    p.setPen(Theme::chromeTextMuted);
    p.drawText(titleRect.adjusted(26, 0, -12, 0), Qt::AlignLeft | Qt::AlignVCenter, title);
    p.restore();
}

void Style::button(QPainter& p, const QRect& rect, Button kind, const Look& look) const
{
    if (kind == Button::Default) {
        // the one primary action: the coral paper button, even inside chrome
        QRectF r = QRectF(rect).adjusted(1, 1, -1, -1);
        if (look.pressed) {
            const qreal dx = r.width() * 0.01, dy = r.height() * 0.01; // scale(0.98)
            r.adjust(dx, dy, -dx, -dy);
        }
        rounded(p, r, Theme::radiusButton, !look.enabled ? Theme::accentDisabled : look.hover || look.pressed ? Theme::accentHover : Theme::accent);
        if (look.focus) {
            rounded(p, r.adjusted(-1.5, -1.5, 1.5, 1.5), Theme::radiusButton + 1.5, QColor(), alpha(Theme::accent, 110));
        }
        return;
    }
    const QRect r = rect.adjusted(1, 1, -1, -1);
    if (kind == Button::Flat) {
        if (look.enabled && (look.hover || look.pressed || look.checked)) {
            box(p, r, look.pressed || look.checked ? Theme::chromeSelected : Theme::chromeHover, QColor());
        }
    } else {
        const QColor fill = !look.enabled ? Theme::chrome1 : look.pressed || look.checked ? Theme::chromeSelected : look.hover ? Theme::chromeHover : Theme::chrome1;
        box(p, r, fill, look.checked && look.enabled ? Theme::accent : Theme::chromeBorder);
    }
    if (look.focus) {
        box(p, r, QColor(), Theme::accent);
    }
}

QColor Style::buttonText(Button kind, const Look& look, const QPalette&) const
{
    if (kind == Button::Default) {
        return look.enabled ? Qt::white : Theme::accentDisabledText;
    }
    if (!look.enabled) {
        return Theme::chromeTextMuted;
    }
    return look.checked || look.pressed ? Theme::chromeTextBright : Theme::chromeText;
}

void Style::field(QPainter& p, const QRect& rect, const Look& look) const
{
    const QColor line = !look.enabled ? Theme::chromeBorder : look.focus ? Theme::accent : look.hover ? Theme::chromeBorderHover : Theme::chromeBorder;
    box(p, rect, Theme::chrome0, line);
}

void Style::view(QPainter& p, const QRect& rect, const Look&) const
{
    box(p, rect, Theme::chrome0, Theme::chromeBorder);
}

void Style::check(QPainter& p, const QRect& rect, Qt::CheckState state, const Look& look) const
{
    QRect r(0, 0, 14, 14);
    r.moveCenter(rect.center());
    box(p, r, Theme::chrome0, look.focus ? Theme::accent : look.hover ? Theme::chromeBorderHover : Theme::chromeTextDisabled);
    const QColor mark = look.enabled ? Theme::accent : Theme::chromeTextDisabled;
    const QRectF b(r);
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
    p.setPen(QPen(look.focus ? Theme::accent : look.hover ? Theme::chromeBorderHover : Theme::chromeTextDisabled, 1));
    p.setBrush(Theme::chrome0);
    p.drawEllipse(r.adjusted(0.5, 0.5, -0.5, -0.5));
    if (on) {
        p.setPen(Qt::NoPen);
        p.setBrush(look.enabled ? Theme::accent : Theme::chromeTextDisabled);
        p.drawEllipse(r.center(), 3.5, 3.5);
    }
    p.restore();
}

void Style::arrow(QPainter& p, const QRect& rect, Qt::ArrowType type, const Look& look) const
{
    chevron(p, QRectF(rect), type, !look.enabled ? Theme::chromeTextDisabled : look.hover ? Theme::chromeTextBright : Theme::chromeTextMuted);
}

void Style::groove(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation, const Look& look) const
{
    p.fillRect(rect, Theme::chromeBorderHover);
    p.fillRect(filled, look.enabled ? Theme::accent : Theme::chromeTextDisabled);
}

void Style::handle(QPainter& p, const QRect& rect, Qt::Orientation, const Look& look) const
{
    const QRectF r = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(look.pressed || look.focus ? QPen(Theme::accent, 2) : QPen(Qt::NoPen)); // coral ring on drag
    p.setBrush(!look.enabled ? Theme::chromeTextDisabled : look.hover ? Qt::white : Theme::chromeTextBright);
    p.drawEllipse(look.pressed || look.focus ? r.adjusted(1, 1, -1, -1) : r);
    p.restore();
}

QSize Style::handleSize(Qt::Orientation) const
{
    return { 12, 12 };
}

void Style::scrollBar(QPainter& p, const QRect&, const QRect& handle, Qt::Orientation orientation, const Look& look) const
{
    const QRect r = orientation == Qt::Horizontal ? handle.adjusted(0, 1, 0, -1) : handle.adjusted(1, 0, -1, 0);
    p.fillRect(r, look.pressed ? alpha(Theme::accent, 160) : look.hover ? alpha(Theme::accent, 90) : Theme::chromeSelected);
}

void Style::progress(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation orientation) const
{
    // a 4px flat track along the bottom edge, so the value text above it stays clear
    constexpr int Track = 4;
    if (orientation == Qt::Horizontal) {
        const QRect track(rect.left(), rect.bottom() - Track + 1, rect.width(), Track);
        p.fillRect(track, Theme::chromeBorderHover);
        p.fillRect(QRect(filled.left(), track.top(), filled.width(), Track), Theme::accent);
    } else {
        const QRect track(rect.left(), rect.top(), Track, rect.height());
        p.fillRect(track, Theme::chromeBorderHover);
        p.fillRect(QRect(track.left(), filled.top(), Track, filled.height()), Theme::accent);
    }
}

void Style::tab(QPainter& p, const QRect& rect, bool selected, const Look& look) const
{
    // flat dark tabs with hairline separators; the active one lifts to the editor surface
    p.fillRect(rect, selected ? Theme::editor : Theme::chrome0);
    p.fillRect(QRect(rect.right(), rect.top(), 1, rect.height()), Theme::chromeBorder);
    if (selected) {
        p.fillRect(QRect(rect.left(), rect.top(), rect.width(), 2), look.enabled ? Theme::accent : Theme::chromeTextDisabled);
    }
}

QColor Style::tabText(bool selected, const Look& look, const QPalette&) const
{
    if (!look.enabled) {
        return Theme::chromeTextDisabled;
    }
    return selected ? Theme::chromeTextBright : look.hover ? Theme::chromeText : Theme::chromeTextMuted;
}

void Style::tabPane(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, Theme::editor);
    p.fillRect(QRect(rect.left(), rect.top(), rect.width(), 1), Theme::chromeBorder);
}

void Style::dial(QPainter& p, const QRect& rect, qreal value, const Look& look) const
{
    const QRectF ring = QRectF(rect).adjusted(6, 6, -6, -6);
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Theme::chromeBorderHover, 3, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(ring, 225 * 16, -270 * 16);
    p.setPen(QPen(look.enabled ? Theme::accent : Theme::chromeTextDisabled, 3, Qt::SolidLine, Qt::FlatCap));
    p.drawArc(ring, 225 * 16, int(-270 * 16 * value));
    const QRectF knob = ring.adjusted(ring.width() * 0.18, ring.height() * 0.18, -ring.width() * 0.18, -ring.height() * 0.18);
    p.setPen(QPen(look.focus ? Theme::accent : look.hover ? Theme::chromeBorderHover : Theme::chromeBorder, 1));
    p.setBrush(look.pressed ? Theme::chromeSelected : Theme::chrome0);
    p.drawEllipse(knob);
    p.setPen(QPen(look.enabled ? Theme::chromeTextBright : Theme::chromeTextDisabled, 2, Qt::SolidLine, Qt::FlatCap));
    p.drawLine(dialPoint(knob, value, knob.width() * 0.12), dialPoint(knob, value, knob.width() * 0.42));
    p.restore();
}

void Style::display(QPainter& p, const QRect& rect) const
{
    // a terminal readout: dark editor chip, coral digits, no bevel or glow
    rounded(p, QRectF(rect), Theme::radiusSmall, Theme::editor, Theme::chromeBorder);
}

QColor Style::displayText() const
{
    return Theme::accent;
}

void Style::menuBar(QPainter& p, const QWidget*, const QRect& rect) const
{
    p.fillRect(rect, Theme::chrome0);
    p.fillRect(QRect(rect.left(), rect.bottom(), rect.width(), 1), Theme::chromeBorder);
}

QColor Style::menuBarText(bool active) const
{
    return active ? Theme::chromeTextBright : Theme::chromeText;
}

void Style::menu(QPainter& p, const QRect& rect) const
{
    box(p, rect, Theme::chrome1, Theme::chromeBorderHover);
}

void Style::highlight(QPainter& p, const QRect& rect, bool inBar) const
{
    p.fillRect(inBar ? rect.adjusted(0, 2, 0, -2) : rect.adjusted(2, 0, -2, 0), inBar ? Theme::chromeHover : Theme::chromeSelected);
}

void Style::selection(QPainter& p, const QRect& rect, const Look& look) const
{
    p.fillRect(rect, look.checked ? Theme::chromeSelected : Theme::chromeHover);
}

void Style::header(QPainter& p, const QRect& rect, const Look& look) const
{
    p.fillRect(rect, look.hover ? Theme::chromeHover : Theme::chrome1);
    p.fillRect(QRect(rect.left(), rect.bottom(), rect.width(), 1), Theme::chromeBorder);
}

void Style::tooltip(QPainter& p, const QRect& rect) const
{
    box(p, rect, Theme::chrome0, Theme::chromeBorder);
}

} // namespace claudecode
