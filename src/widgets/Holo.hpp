#pragma once

#include "Icons.hpp"

#include <QWidget>

class QScrollArea;
class QToolButton;
class QVBoxLayout;

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

/// Rounded square icon button.
QToolButton* roundButton(Icon icon, const QString& toolTip, QWidget* parent = nullptr);

} // namespace Holo
