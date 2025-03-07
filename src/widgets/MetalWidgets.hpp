#pragma once

#include "widgets/Metal.hpp"

#include <QAbstractButton>
#include <QWidget>

class QLabel;
class QScrollArea;

/// Metallic pill button (ON / AUTO / PRESETS, PL / ML style); "active" lights it blue.
class CapsuleButton : public QAbstractButton {
    Q_OBJECT
public:
    explicit CapsuleButton(const QString& text, Metal::Glyph glyph = Metal::Glyph::None, QWidget* parent = nullptr);
    void setActive(bool active);
    bool isActive() const { return m_active; }
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    Metal::Glyph m_glyph;
    bool m_active = false;
    bool m_hover = false;
};

/// Circular, bevel-edged transport button; the active one gets the blue accent.
class RoundButton : public QAbstractButton {
    Q_OBJECT
public:
    RoundButton(Metal::Glyph glyph, int diameter, QWidget* parent = nullptr);
    void setGlyph(Metal::Glyph glyph);
    void setActive(bool active);
    bool isActive() const { return m_active; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    Metal::Glyph m_glyph;
    bool m_active = false;
    bool m_hover = false;
};

/// Illuminated cobalt LCD panel (container).
class LcdPanel : public QWidget {
public:
    explicit LcdPanel(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};

/// Horizontal position slider with a metallic capsule thumb (0 .. maximum).
class PositionSlider : public QWidget {
    Q_OBJECT
public:
    explicit PositionSlider(QWidget* parent = nullptr);
    void setMaximum(int maximum);
    void setValue(int value);
    int value() const { return m_value; }
    QSize sizeHint() const override { return QSize(200, 20); }

signals:
    void valueChanged(int value);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QRectF track() const;
    QRectF thumb() const;
    void seek(qreal x);

    int m_maximum = 0;
    int m_value = 0;
    bool m_dragging = false;
};

namespace MetalUi {

/// Glowing LCD label.
QLabel* lcdLabel(const QString& text, bool title = false);
/// Frameless, transparent, vertically scrolling area around @p content.
QScrollArea* scrollArea(QWidget* content);

} // namespace MetalUi
