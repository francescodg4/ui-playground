#pragma once

#include <QTimer>
#include <QWidget>

class LogRow;
class PixelKey;

/// Messages received by the PDA, grouped by day under banners, each with a PLAY key.
class LogPage : public QWidget {
public:
    explicit LogPage(QWidget* parent = nullptr);

    /// Number of entries not read yet (shown as the tab badge).
    static int unreadCount();

private:
    void togglePlayback(PixelKey* key, LogRow* row, const QString& text);
    void setPlaying(PixelKey* key, LogRow* row, bool playing);

    PixelKey* m_playing = nullptr;
    LogRow* m_playingRow = nullptr;
    QTimer m_playback;
};
