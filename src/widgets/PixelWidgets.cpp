#include "PixelWidgets.hpp"

#include "widgets/Pixel.hpp"

#include <QFontMetrics>
#include <QScrollArea>

using Pixel::Scale;

// ---- label ---------------------------------------------------------------------------------------

PixelLabel::PixelLabel(const QString& text, Tone tone, bool heading, QWidget* parent)
    : QWidget(parent)
    , m_text(text)
    , m_tone(tone)
    , m_heading(heading)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void PixelLabel::setText(const QString& text)
{
    m_text = text;
    updateGeometry();
    update();
}

void PixelLabel::setTone(Tone tone)
{
    m_tone = tone;
    update();
}

void PixelLabel::setWordWrap(bool wrap)
{
    m_wrap = wrap;
    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    policy.setHeightForWidth(wrap);
    setSizePolicy(wrap ? policy : QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    updateGeometry();
}

void PixelLabel::setAlignment(Qt::Alignment alignment)
{
    m_alignment = alignment;
    update();
}

QSize PixelLabel::sizeHint() const
{
    const QFontMetrics fm(Pixel::font(m_heading));
    if (m_wrap) {
        return QSize(200 * Scale, heightForWidth(200 * Scale));
    }
    return QSize((fm.horizontalAdvance(m_text) + 2) * Scale, (fm.height() + 2) * Scale);
}

QSize PixelLabel::minimumSizeHint() const
{
    return m_wrap ? QSize(40 * Scale, heightForWidth(40 * Scale)) : sizeHint();
}

int PixelLabel::heightForWidth(int width) const
{
    return (Pixel::textHeight(m_text, width / Scale - 1, m_heading) + 1) * Scale;
}

void PixelLabel::paintEvent(QPaintEvent*)
{
    Pixel::Canvas canvas(this);
    QPainter& p = canvas.p();
    p.setFont(Pixel::font(m_heading));
    const QColor color = m_tone == Tone::Light ? Theme::lightText : m_tone == Tone::Dim ? Theme::dimText : Theme::text;
    const QColor shadow = m_tone == Tone::Light ? Theme::lightTextShadow : Theme::textShadow;
    const int flags = int(m_alignment) | (m_wrap ? Qt::TextWordWrap : 0);
    Pixel::text(p, canvas.rect().adjusted(0, 0, -1, -1), flags, m_text, color, shadow);
}

// ---- key -----------------------------------------------------------------------------------------

PixelKey::PixelKey(const QString& label, const QColor& face, QWidget* parent)
    : QAbstractButton(parent)
    , m_face(face)
{
    setText(label);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    setFixedSize(sizeHint());
}

void PixelKey::setLabel(const QString& label)
{
    setText(label);
    setFixedSize(sizeHint());
    update();
}

void PixelKey::setFace(const QColor& face)
{
    m_face = face;
    update();
}

QSize PixelKey::sizeHint() const
{
    return QSize(Pixel::keyWidth(text()) * Scale, Pixel::KeyHeight * Scale);
}

void PixelKey::paintEvent(QPaintEvent*)
{
    Pixel::Canvas canvas(this);
    Pixel::key(canvas.p(), canvas.rect(), m_face, text(), isDown(), isEnabled());
}

// ---- containers ----------------------------------------------------------------------------------

PaperBox::PaperBox(QWidget* parent)
    : QWidget(parent)
{
}

void PaperBox::paintEvent(QPaintEvent*)
{
    Pixel::Canvas canvas(this);
    Pixel::textBox(canvas.p(), canvas.rect());
}

DeckPanel::DeckPanel(QWidget* parent)
    : QWidget(parent)
{
}

void DeckPanel::paintEvent(QPaintEvent*)
{
    Pixel::Canvas canvas(this);
    Pixel::deck(canvas.p(), canvas.rect());
}

// ---- banner & sprite ------------------------------------------------------------------------------

BannerStrip::BannerStrip(const QString& caption, Pixel::Wallpaper wallpaper, QWidget* parent)
    : QWidget(parent)
    , m_caption(caption)
    , m_wallpaper(wallpaper)
{
    setFixedHeight(16 * Scale);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void BannerStrip::paintEvent(QPaintEvent*)
{
    Pixel::Canvas canvas(this);
    const QRect r = canvas.rect();
    Pixel::banner(canvas.p(), QRect(r.left(), r.top(), std::min(r.width(), 180), r.height()), m_wallpaper, m_caption);
}

SpriteLabel::SpriteLabel(Icon icon, int size, const QColor& accent, QWidget* parent)
    : QWidget(parent)
    , m_icon(icon)
    , m_size(size)
    , m_accent(accent)
{
    setFixedSize(size * Scale, size * Scale);
}

void SpriteLabel::setAccent(const QColor& accent)
{
    m_accent = accent;
    update();
}

void SpriteLabel::paintEvent(QPaintEvent*)
{
    Pixel::Canvas canvas(this);
    canvas.p().drawImage(0, 0, Pixel::sprite(m_icon, m_size, m_accent));
}

namespace PixelUi {

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

} // namespace PixelUi
