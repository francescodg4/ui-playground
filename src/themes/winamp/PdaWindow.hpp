#pragma once

#include "PdaShell.hpp"

#include <QWidget>

class PhotoLibrary;
class QStackedWidget;

namespace winamp {

class MenuStrip;
class Telemetry;

/// The PDA as a Winamp Modern skinned window: title bar, textual menu selecting the page,
/// the telemetry LCD and the page itself inside the metal frame. Pages switch instantly.
class PdaWindow : public PdaShell {
    Q_OBJECT
public:
    explicit PdaWindow(PhotoLibrary* photos, QWidget* parent = nullptr);

    int pageCount() const override;
    int currentPage() const override;
    void setPage(int index) override;
    void showPhotoViewer() override;
    int pageIndex(QWidget* page) const;

    /// Visualizer motion (off for reduced motion and screenshots).
    void setAnimationsEnabled(bool enabled) override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void addPage(const QString& menuText, QWidget* page);
    void showPage(int index);

    MenuStrip* m_menu = nullptr;
    Telemetry* m_telemetry = nullptr;
    QStackedWidget* m_stack = nullptr;
};

} // namespace winamp
