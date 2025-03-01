#pragma once

#include <QWidget>

/// Inventory grid on the left, equipment slots around the diver on the right.
class InventoryPage : public QWidget {
public:
    explicit InventoryPage(QWidget* parent = nullptr);
};
