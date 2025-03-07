#pragma once

#include <QWidget>

namespace glass {

/// Craftable items grouped by category, each shown as a round token with its name.
class BlueprintsPage : public QWidget {
public:
    explicit BlueprintsPage(QWidget* parent = nullptr);
};

} // namespace glass
