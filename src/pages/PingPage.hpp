#pragma once

#include <QWidget>

/// Tracked signals: visibility toggle, icon, name and a colour picker per ping.
class PingPage : public QWidget {
public:
    explicit PingPage(QWidget* parent = nullptr);
};
