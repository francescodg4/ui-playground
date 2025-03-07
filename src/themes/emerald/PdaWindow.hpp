#pragma once

#include "Icons.hpp"
#include "themes/emerald/widgets/Pixel.hpp"

#include "PdaShell.hpp"

#include <QWidget>

class PhotoLibrary;
class QStackedWidget;

namespace emerald {

class BoxHeader;

/// The PDA as a storage-box screen: box header menu on top, the current page drawn over its
/// wallpaper container. Pages switch instantly (no transitions).
class PdaWindow : public PdaShell {
    Q_OBJECT
public:
    explicit PdaWindow(PhotoLibrary* photos, QWidget* parent = nullptr);

    int pageCount() const override;
    int currentPage() const override;
    void setPage(int index) override;
    void showPhotoViewer() override;
    int pageIndex(QWidget* page) const;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void addPage(Icon icon, const QString& name, Pixel::Wallpaper wallpaper, QWidget* page);

    BoxHeader* m_header = nullptr;
    QStackedWidget* m_stack = nullptr;
};

} // namespace emerald
