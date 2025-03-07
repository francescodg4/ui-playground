#pragma once

#include "Icons.hpp"

#include <QTimer>
#include "PdaShell.hpp"

#include <QWidget>

class PhotoLibrary;
class QStackedWidget;

namespace glass {

class PageTransition;
class TabBar;

/// The PDA: a live ambient canvas under glass layers, with a tab bar selecting one page at a time.
class PdaWindow : public PdaShell {
    Q_OBJECT
public:
    explicit PdaWindow(PhotoLibrary* photos, QWidget* parent = nullptr);
    ~PdaWindow() override;

    int pageCount() const override;
    int currentPage() const override;
    void setPage(int index) override;
    void showPhotoViewer() override;
    int pageIndex(QWidget* page) const;

    /// Page transitions on/off (off for reduced motion and screenshots).
    void setAnimationsEnabled(bool enabled) override { m_animate = enabled; }

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

} // namespace glass
