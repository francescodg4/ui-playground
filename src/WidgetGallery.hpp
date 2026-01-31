#pragma once

#include <QMainWindow>
#include <QTimer>

class QAction;
class QActionGroup;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLCDNumber;
class QListWidget;
class QProgressBar;
class QTreeWidget;

/// The most used standard widgets (buttons, inputs, ranges, item views, displays, menus) in one
/// window, drawn by the WidgetStyle of the selected interface. Choosing an interface restyles the
/// whole application at once.
class WidgetGallery : public QMainWindow {
    Q_OBJECT
public:
    explicit WidgetGallery(QWidget* parent = nullptr);

    QString theme() const { return m_theme; }
    /// Draws the application's standard widgets with the design of theme @p id.
    void setTheme(const QString& id);
    /// Uses the dark mode of the designs that have one.
    void setDark(bool dark);
    bool dark() const { return m_dark; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QGroupBox* interfaceBox();
    QGroupBox* buttonsBox();
    QGroupBox* inputBox();
    QGroupBox* rangesBox();
    QGroupBox* viewsBox();
    QGroupBox* telemetryBox();
    void select(const QString& id); ///< user choice: applied and remembered
    void setWidgetsEnabled(bool enabled);
    void tintIcons(); ///< item icons follow the text colour of the interface

    QString m_theme;
    QComboBox* m_selector = nullptr;
    QLabel* m_description = nullptr;
    QCheckBox* m_disable = nullptr;
    QCheckBox* m_darkMode = nullptr;
    QAction* m_darkAction = nullptr;
    bool m_dark = false;
    QActionGroup* m_themeActions = nullptr;
    QList<QGroupBox*> m_boxes; ///< the showcase, disabled together
    QListWidget* m_list = nullptr;
    QTreeWidget* m_tree = nullptr;
    QProgressBar* m_scan = nullptr;
    QLCDNumber* m_clock = nullptr;
    QTimer m_ticker;
};
