#pragma once

#include "Icons.hpp"
#include "widgets/Pixel.hpp"

#include <QWidget>

class BoxHeader;
class PhotoLibrary;
class QStackedWidget;

/// The PDA as a storage-box screen: box header menu on top, the current page drawn over its
/// wallpaper container. Pages switch instantly (no transitions).
class PdaWindow : public QWidget {
    Q_OBJECT
public:
    explicit PdaWindow(PhotoLibrary* photos, QWidget* parent = nullptr);

    int pageCount() const;
    int currentPage() const;
    void setPage(int index);
    int pageIndex(QWidget* page) const;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void addPage(Icon icon, const QString& name, Pixel::Wallpaper wallpaper, QWidget* page);

    BoxHeader* m_header = nullptr;
    QStackedWidget* m_stack = nullptr;
};
