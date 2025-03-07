#include "Theme.hpp"

namespace Theme {

QString styleSheet()
{
    // square, bi-colour pixel scroll bars snapped to the 2px grid
    return QStringLiteral(R"(
QWidget { color: #404040; background: transparent; font-family: "DejaVu Sans Mono"; }
QToolTip { color: #404040; background: #f8f8e0; border: 2px solid #282828; padding: 2px 4px; }

QScrollArea, QListView, QTreeView { border: none; background: transparent; }

QScrollBar:vertical { width: 12px; margin: 0; background: #f8f8e0; border: 2px solid #282828; }
QScrollBar::handle:vertical { min-height: 24px; background: #389888; border: 2px solid #185048; margin: 0; }
QScrollBar:horizontal { height: 12px; margin: 0; background: #f8f8e0; border: 2px solid #282828; }
QScrollBar::handle:horizontal { min-width: 24px; background: #389888; border: 2px solid #185048; margin: 0; }
QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }
QScrollBar::add-page, QScrollBar::sub-page { background: none; }

QMessageBox, QFileDialog { background: #f8f8e0; }
QMessageBox QLabel { color: #404040; }
QMessageBox QPushButton, QFileDialog QPushButton {
    min-width: 70px; padding: 4px 10px; color: #404040;
    border: 2px solid #282828; background: #f0d860;
}
)");
}

} // namespace Theme
