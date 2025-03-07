#pragma once

#include <QWidget>

namespace emerald {

/// Storage box grid on the left, the equipped items as a party deck on the right.
class InventoryPage : public QWidget {
public:
    explicit InventoryPage(QWidget* parent = nullptr);
};

} // namespace emerald
