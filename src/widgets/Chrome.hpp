#pragma once

#include <QWidget>

/// Main title bar: dark metallic gradient with reflection ridges, centred branding, window
/// controls on the right (minimise, maximise / windowshade, close). Drag it to move the window.
class TitleBar : public QWidget {
public:
    explicit TitleBar(const QString& title, QWidget* parent = nullptr);
    QSize sizeHint() const override { return QSize(400, 26); }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QRectF controlRect(int index) const; ///< 0 minimise, 1 maximise, 2 close
    int controlAt(const QPointF& pos) const;

    QString m_title;
    int m_hover = -1;
    int m_pressed = -1;
};

/// Inline textual menu under the title bar; each entry opens a page (Alt + underlined letter).
class MenuStrip : public QWidget {
    Q_OBJECT
public:
    explicit MenuStrip(QWidget* parent = nullptr);

    void addItem(const QString& textWithMnemonic);
    int count() const { return int(m_items.size()); }
    int currentIndex() const { return m_current; }
    void setCurrentIndex(int index);
    QChar mnemonic(int index) const;
    QString itemText(int index) const { return QString(m_items.value(index)).remove(u'&'); }

    QSize sizeHint() const override { return QSize(400, 28); }

signals:
    void currentChanged(int index);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QRectF itemRect(int index) const;
    int itemAt(const QPointF& pos) const;

    QStringList m_items;
    int m_current = -1;
    int m_hover = -1;
};
