#pragma once

#include <QPixmap>
#include <QVariantAnimation>
#include <QWidget>

namespace glass {

/// Overlay that animates from one page snapshot to another: the pages slide a short
/// distance in the direction of travel while cross-fading.
class PageTransition : public QWidget {
    Q_OBJECT
public:
    explicit PageTransition(QWidget* parent = nullptr);

    /// @p direction: +1 moving to a later page, -1 to an earlier one.
    void run(const QPixmap& from, const QPixmap& to, int direction);
    bool isRunning() const;
    void finish();

signals:
    void finished();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap m_from;
    QPixmap m_to;
    int m_direction = 1;
    qreal m_progress = 0;
    QVariantAnimation m_animation;
};

} // namespace glass
