#pragma once

#include <QElapsedTimer>
#include <QTimer>
#include <QWidget>

namespace winamp {

/// Primary telemetry display (LCD / VFD panel): 7-segment clock, counters, status badge,
/// spectrum visualizer and a dot-matrix status line. One instance, reachable from every page.
class Telemetry : public QWidget {
public:
    explicit Telemetry(QWidget* parent = nullptr);
    ~Telemetry() override;

    /// Text of the status line (page status, selection, playing log entry...).
    static void report(const QString& message);
    /// Log playback: runs the elapsed clock and livens the visualizer.
    static void setPlaying(bool playing);

    void setPage(const QString& name, const QString& status);
    void setCounter(const QString& label, int value);
    /// Visualizer motion (off for reduced motion and screenshots).
    void setAnimated(bool animated);

    QSize sizeHint() const override { return QSize(600, 92); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void tick();

    QString m_page;
    QString m_message;
    bool m_playing = false;
    QElapsedTimer m_playClock;
    QList<QPair<QString, int>> m_counters;
    qreal m_phase = 0; ///< drives the idle visualizer ripple
    QVector<qreal> m_bands;
    QVector<qreal> m_peaks;
    QTimer m_timer;
};

} // namespace winamp
