#pragma once

#include <QTimer>
#include <QWidget>

namespace winamp {

class LogRow;
class RoundButton;

/// Messages received by the PDA as a playlist on the LCD; playing one drives the telemetry
/// (elapsed clock, status line, visualizer).
class LogPage : public QWidget {
public:
    explicit LogPage(QWidget* parent = nullptr);

    /// Number of entries not read yet.
    static int unreadCount();

private:
    void togglePlayback(RoundButton* button, LogRow* row, const QString& text);
    void setPlaying(RoundButton* button, LogRow* row, bool playing);

    RoundButton* m_playing = nullptr;
    LogRow* m_playingRow = nullptr;
    QTimer m_playback;
};

} // namespace winamp
