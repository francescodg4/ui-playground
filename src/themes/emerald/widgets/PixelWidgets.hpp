#pragma once

#include "Icons.hpp"
#include "themes/emerald/Theme.hpp"
#include "themes/emerald/widgets/Pixel.hpp"

#include <QAbstractButton>
#include <QWidget>

class QScrollArea;

namespace emerald {

/// Text in the pixel font with a bi-colour drop shadow.
class PixelLabel : public QWidget {
public:
    enum class Tone {
        Dark, ///< dark text, light shadow (on cream, metal, light wallpapers)
        Light, ///< white text, dark shadow (on green decks, blue screens)
        Dim,
    };

    explicit PixelLabel(const QString& text = {}, Tone tone = Tone::Dark, bool heading = false, QWidget* parent = nullptr);

    void setText(const QString& text);
    QString text() const { return m_text; }
    void setTone(Tone tone);
    void setWordWrap(bool wrap);
    void setAlignment(Qt::Alignment alignment);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    bool hasHeightForWidth() const override { return m_wrap; }
    int heightForWidth(int width) const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_text;
    Tone m_tone;
    bool m_heading;
    bool m_wrap = false;
    Qt::Alignment m_alignment = Qt::AlignLeft | Qt::AlignVCenter;
};

/// Beveled capsule key (SELECT / B BUTTON / OK-START style) with a physical press state.
class PixelKey : public QAbstractButton {
    Q_OBJECT
public:
    explicit PixelKey(const QString& label, const QColor& face = Theme::keyYellow, QWidget* parent = nullptr);

    void setLabel(const QString& label);
    void setFace(const QColor& face);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QColor m_face;
};

/// Cream lined-paper container with a double-line border.
class PaperBox : public QWidget {
public:
    explicit PaperBox(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};

/// Teal party-deck container.
class DeckPanel : public QWidget {
public:
    explicit DeckPanel(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};

/// Pill-cornered header banner with landscape art and a caption plate.
class BannerStrip : public QWidget {
public:
    BannerStrip(const QString& caption, Pixel::Wallpaper wallpaper, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_caption;
    Pixel::Wallpaper m_wallpaper;
};

/// An icon shown as an outlined pixel sprite.
class SpriteLabel : public QWidget {
public:
    SpriteLabel(Icon icon, int size, const QColor& accent = Qt::white, QWidget* parent = nullptr);
    void setAccent(const QColor& accent);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Icon m_icon;
    int m_size;
    QColor m_accent;
};

namespace PixelUi {

/// Frameless, transparent, vertically scrolling area around @p content.
QScrollArea* scrollArea(QWidget* content);

} // namespace PixelUi

} // namespace emerald
