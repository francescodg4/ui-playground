#pragma once

#include <QWidget>

class MenuStrip;
class PhotoLibrary;
class QStackedWidget;
class Telemetry;

/// The PDA as a Winamp Modern skinned window: title bar, textual menu selecting the page,
/// the telemetry LCD and the page itself inside the metal frame. Pages switch instantly.
class PdaWindow : public QWidget {
    Q_OBJECT
public:
    explicit PdaWindow(PhotoLibrary* photos, QWidget* parent = nullptr);

    int pageCount() const;
    int currentPage() const;
    void setPage(int index);
    int pageIndex(QWidget* page) const;

    /// Marquee scrolling and visualizer motion (off for reduced motion and screenshots).
    void setAnimationsEnabled(bool enabled);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void addPage(const QString& menuText, QWidget* page);
    void showPage(int index);

    MenuStrip* m_menu = nullptr;
    Telemetry* m_telemetry = nullptr;
    QStackedWidget* m_stack = nullptr;
};
