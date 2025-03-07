#pragma once

#include <QWidget>

/// Tracked signals: show/hide key, sprite, name and a colour swatch picker per ping.
class PingPage : public QWidget {
public:
    explicit PingPage(QWidget* parent = nullptr);
};
