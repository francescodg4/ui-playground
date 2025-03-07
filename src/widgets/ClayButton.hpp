#pragma once

#include "Icons.hpp"

#include <QAbstractButton>
#include <QTimer>

/// Tactile clay button: icon (and optional text) on a soft volumetric surface that
/// compresses while pressed. Hover and "active" states radiate a glow instead of an outline.
class ClayButton : public QAbstractButton {
    Q_OBJECT
public:
    explicit ClayButton(Icon icon, QWidget* parent = nullptr);

    void setIconShape(Icon icon);
    void setIconColor(const QColor& color);
    void setGlowColor(const QColor& color);
    /// Visual size of the clay body; the widget adds room around it for shadow and glow.
    void setBodySize(const QSize& size);
    /// Active processing: pulsing radiance plus a rotating gradient edge.
    void setActive(bool active);
    bool isActive() const { return m_active; }

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QRectF bodyRect() const;

    Icon m_icon;
    QColor m_iconColor = Qt::white;
    QColor m_glow;
    QSize m_body { 36, 36 };
    bool m_hover = false;
    bool m_active = false;
    QTimer m_frames;
};
