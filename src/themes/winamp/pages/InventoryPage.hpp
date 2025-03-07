#pragma once

#include <QWidget>

namespace winamp {

/// Storage grid on an LCD panel, equipped items as round metallic buttons around the diver.
class InventoryPage : public QWidget {
public:
    explicit InventoryPage(QWidget* parent = nullptr);
};

} // namespace winamp
