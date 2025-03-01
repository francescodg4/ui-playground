#pragma once

#include "Icons.hpp"

#include <QElapsedTimer>
#include <QTimer>
#include <QWidget>

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

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void addPage(Icon icon, const QString& name, QWidget* page);
    QRectF screenRect() const;
    void paintScreen(QPainter& p, const QRectF& screen);

    struct Speck {
        qreal x, y, radius, speed, alpha, phase;
    };

    TabBar* m_tabs = nullptr;
    QStackedWidget* m_stack = nullptr;
    QList<Speck> m_specks;
    QTimer m_animation;
    QElapsedTimer m_clock;
};
