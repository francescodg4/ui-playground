#pragma once

#include <QTimer>
#include <QWidget>

class QToolButton;

/// Messages received by the PDA, grouped by day, each with a play button.
class LogPage : public QWidget {
public:
    explicit LogPage(QWidget* parent = nullptr);

    /// Number of entries not read yet (shown as the tab badge).
    static int unreadCount();

private:
    void togglePlayback(QToolButton* button, const QString& text);
    void setPlaying(QToolButton* button, bool playing);

    QToolButton* m_playing = nullptr;
    QTimer m_playback;
};
