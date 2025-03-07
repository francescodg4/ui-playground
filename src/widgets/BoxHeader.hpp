#pragma once

#include "Icons.hpp"
#include "widgets/Pixel.hpp"

#include <QWidget>

/// Top menu in the style of the storage-box header: a row of item tiles (the glove cursor
/// points at the current one) above the box-name banner with bi-colour arrows either side.
class BoxHeader : public QWidget {
    Q_OBJECT
public:
    explicit BoxHeader(QWidget* parent = nullptr);

    void addTab(Icon icon, const QString& name, Pixel::Wallpaper wallpaper);
    void setBadge(int index, int count);
    int count() const { return int(m_tabs.size()); }
    int currentIndex() const { return m_current; }
    void setCurrentIndex(int index);
    Pixel::Wallpaper wallpaper(int index) const { return m_tabs.value(index).wallpaper; }

    QSize sizeHint() const override;

signals:
    void currentChanged(int index);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool event(QEvent* event) override;

private:
    struct Tab {
        Icon icon = Icon::Person;
        QString name;
        Pixel::Wallpaper wallpaper = Pixel::Wallpaper::Simple;
        int badge = 0;
    };
    enum Target { None = -1, PrevArrow = -2, NextArrow = -3 };

    QRect tileRect(int index) const; ///< in pixels
    QRect bannerRect() const;
    QRect arrowRect(bool next) const;
    int hit(const QPoint& devicePos) const;

    QList<Tab> m_tabs;
    int m_current = -1;
    int m_hover = None;
    int m_pressed = None;
};
