#pragma once

#include <QWidget>

namespace emerald {

/// Craftable items grouped by category under header banners, shown as sprite tiles.
class BlueprintsPage : public QWidget {
public:
    explicit BlueprintsPage(QWidget* parent = nullptr);
};

} // namespace emerald
