#pragma once

#include "Icons.hpp"

#include <QWidget>

namespace glass {

/// Row of slanted icon tabs at the top of the PDA, with optional notification badges.
class TabBar : public QWidget {
    Q_OBJECT
public:
    explicit TabBar(QWidget* parent = nullptr);

    void addTab(Icon icon, const QString& toolTip);
    void setBadge(int index, int count);
    int badge(int index) const;
    int count() const { return int(m_tabs.size()); }
    int currentIndex() const { return m_current; }
    void setCurrentIndex(int index);

    QSize sizeHint() const override;

signals:
    void currentChanged(int index);

protected:
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    struct Tab {
        Icon icon;
        QString toolTip;
        int badge = 0;
    };

    QRectF tabRect(int index) const;
    int tabAt(const QPointF& pos) const;

    QList<Tab> m_tabs;
    int m_current = -1;
    int m_hover = -1;
    int m_pressed = -1;
};

} // namespace glass
