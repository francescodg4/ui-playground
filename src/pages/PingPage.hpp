#pragma once

#include <QWidget>

/// Tracked signals: ON/OFF capsule, icon in an LCD well, name and indicator LEDs picking its colour.
class PingPage : public QWidget {
public:
    explicit PingPage(QWidget* parent = nullptr);
};
