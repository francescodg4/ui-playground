#pragma once

#include "Icons.hpp"

#include <QElapsedTimer>
#include <QTimer>
#include <QWidget>

class PageTransition;
class PhotoLibrary;
class QStackedWidget;
class TabBar;

/// The PDA: a glowing holographic screen with a tab bar selecting one page at a time.
class PdaWindow : public QWidget {
    Q_OBJECT
public:
    explicit PdaWindow(PhotoLibrary* photos, QWidget* parent = nullptr);

    int pageCount() const;
    int currentPage() const;
    void setPage(int index);
    int pageIndex(QWidget* page) const;

    /// Page transitions on/off (off for reduced motion and screenshots).
    void setAnimationsEnabled(bool enabled) { m_animate = enabled; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void addPage(Icon icon, const QString& name, QWidget* page);
    void showPage(int index);
    QRectF screenRect() const;
    void paintScreen(QPainter& p, const QRectF& screen);

    struct Speck {
        qreal x, y, radius, speed, alpha, phase;
    };

    TabBar* m_tabs = nullptr;
    QStackedWidget* m_stack = nullptr;
    PageTransition* m_transition = nullptr;
    bool m_animate = true;
    QList<Speck> m_specks;
    QTimer m_animation;
    QElapsedTimer m_clock;
};
