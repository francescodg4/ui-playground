#pragma once

#include "Icons.hpp"

#include <QTimer>
#include <QWidget>

class PageTransition;
class PhotoLibrary;
class QStackedWidget;
class TabBar;

/// The PDA: a live ambient canvas under glass layers, with a tab bar selecting one page at a time.
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
    void resizeEvent(QResizeEvent* event) override;

private:
    void addPage(Icon icon, const QString& name, QWidget* page);
    void showPage(int index);
    QRectF screenRect() const;

    TabBar* m_tabs = nullptr;
    QStackedWidget* m_stack = nullptr;
    PageTransition* m_transition = nullptr;
    bool m_animate = true;
    QTimer m_frames; ///< drives the ambient canvas
};
