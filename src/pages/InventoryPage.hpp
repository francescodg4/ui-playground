#pragma once

#include <QWidget>

/// Storage box grid on the left, the equipped items as a party deck on the right.
class InventoryPage : public QWidget {
public:
    explicit InventoryPage(QWidget* parent = nullptr);
};
