#pragma once

#include <QWidget>

namespace winamp {

/// Craftable items as a numbered playlist on the LCD, grouped by category.
class BlueprintsPage : public QWidget {
    Q_OBJECT
public:
    explicit BlueprintsPage(QWidget* parent = nullptr);
};

} // namespace winamp
