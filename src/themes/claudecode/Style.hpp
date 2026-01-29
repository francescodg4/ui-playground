#pragma once

#include "WidgetStyle.hpp"

namespace claudecode {

/// The standard widgets in the Claude x VS Code hybrid: square dark chrome in warm neutrals (panels
/// titled like sidebar sections, sunken fields, compact trees and tables), rich text on rounded cream
/// paper, and one coral accent for the default button, marks, fills, focus and the active tab.
class Style : public WidgetStyle {
public:
    Style();

    QFont font() const override;
    QPalette standardPalette() const override;

    // rich text reads on paper: the text edit gets its own frame and text colour
    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;
    void polish(QWidget* widget) override;
    void unpolish(QWidget* widget) override;
    using WidgetStyle::polish;
    using WidgetStyle::unpolish;

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
};

} // namespace claudecode
