#pragma once

#include "Icons.hpp"

#include <QWidget>

class QLayout;
class QScrollArea;
class QVBoxLayout;

namespace glass {

class ClayButton;

/// Section heading: uppercase cyan text over a gold swoosh.
class SectionTitle : public QWidget {
public:
    explicit SectionTitle(const QString& text, QWidget* parent = nullptr);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_text;
};

namespace Holo {

/// Vertical page layout (no margins) starting with a centred @ref SectionTitle.
QVBoxLayout* pageLayout(QWidget* page, const QString& title);

/// Frameless, transparent, vertically scrolling area around @p content.
QScrollArea* scrollArea(QWidget* content);

/// Bento card: frosted glass container around @p content (padding and gap from the theme).
QWidget* card(QLayout* content);

/// Round clay icon button.
ClayButton* roundButton(Icon icon, const QString& toolTip, QWidget* parent = nullptr);

/// Clay pill with an icon and a label.
ClayButton* pillButton(Icon icon, const QString& text, QWidget* parent = nullptr);

} // namespace Holo

} // namespace glass
