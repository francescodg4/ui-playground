#pragma once

#include "WidgetStyle.hpp"
#include "themes/proxmox/Theme.hpp"

namespace proxmox {

/// The standard widgets in the Proxmox VE web console (ExtJS "Crisp"): square white panels with a
/// pale header strip and a blue title on a light-grey page, white toolbar buttons and fields with 2px
/// radii, solid-blue primary actions and active tabs, the resource tree's outlined selection, thin
/// usage bars, and scroll bars with step buttons. It comes in the light Crisp theme and a dark mode.
class Style : public WidgetStyle {
public:
    explicit Style(bool dark = false);

    QFont font() const override;
    QPalette standardPalette() const override;

    // grid headers carry muted text
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;

    void window(QPainter& p, const QWidget* widget, const QRect& rect) const override;
    void card(QPainter& p, const QWidget* widget, const QRect& rect, const QRect& titleRect, const QString& title) const override;
    void button(QPainter& p, const QRect& rect, Button kind, const Look& look) const override;
    QColor buttonText(Button kind, const Look& look, const QPalette& palette) const override;
    void field(QPainter& p, const QRect& rect, const Look& look) const override;
    void view(QPainter& p, const QRect& rect, const Look& look) const override;
    void check(QPainter& p, const QRect& rect, Qt::CheckState state, const Look& look) const override;
    void radio(QPainter& p, const QRect& rect, bool on, const Look& look) const override;
    void arrow(QPainter& p, const QRect& rect, Qt::ArrowType type, const Look& look) const override;
    void groove(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation orientation, const Look& look) const override;
    void handle(QPainter& p, const QRect& rect, Qt::Orientation orientation, const Look& look) const override;
    QSize handleSize(Qt::Orientation orientation) const override;
    void scrollBar(QPainter& p, const QRect& groove, const QRect& handle, Qt::Orientation orientation, const Look& look) const override;
    void progress(QPainter& p, const QRect& rect, const QRect& filled, Qt::Orientation orientation) const override;
    void tab(QPainter& p, const QRect& rect, bool selected, const Look& look) const override;
    QColor tabText(bool selected, const Look& look, const QPalette& palette) const override;
    void tabPane(QPainter& p, const QWidget* widget, const QRect& rect) const override;
    void dial(QPainter& p, const QRect& rect, qreal value, const Look& look) const override;
    void display(QPainter& p, const QRect& rect) const override;
    QColor displayText() const override;
    void menuBar(QPainter& p, const QWidget* widget, const QRect& rect) const override;
    QColor menuBarText(bool active) const override;
    void menu(QPainter& p, const QRect& rect) const override;
    void highlight(QPainter& p, const QRect& rect, bool inBar) const override;
    void selection(QPainter& p, const QRect& rect, const Look& look) const override;
    void header(QPainter& p, const QRect& rect, const Look& look) const override;
    void tooltip(QPainter& p, const QRect& rect) const override;

private:
    const Theme::Colors& c;
};

} // namespace proxmox
