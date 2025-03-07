#include "Theme.hpp"

namespace Theme {

QString styleSheet()
{
    return QStringLiteral(R"(
QWidget { color: #1c2433; background: transparent; font-family: "Tahoma", "Segoe UI", "DejaVu Sans"; }
QToolTip { color: #1c2433; background: #eef1f5; border: 1px solid #6a7486; padding: 2px 5px; }

QScrollArea, QListView, QTreeView { border: none; background: transparent; }
QLabel#lcd { color: #70b0ff; font-family: "Courier New", "Consolas", "DejaVu Sans Mono"; }
QLabel#lcdTitle { color: #70b0ff; font-family: "Courier New", "Consolas", "DejaVu Sans Mono"; font-weight: bold; font-size: 14pt; }
QLabel#dim { color: #5a6478; }

QScrollBar:vertical { width: 14px; margin: 0; background: #2a3446; border: 1px solid #404858; border-radius: 7px; }
QScrollBar::handle:vertical {
    min-height: 30px; margin: 1px; border-radius: 6px; border: 1px solid #6a7486;
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #f4f6fa, stop:1 #b8c0cc);
}
QScrollBar:horizontal { height: 14px; margin: 0; background: #2a3446; border: 1px solid #404858; border-radius: 7px; }
QScrollBar::handle:horizontal {
    min-width: 30px; margin: 1px; border-radius: 6px; border: 1px solid #6a7486;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f4f6fa, stop:1 #b8c0cc);
}
QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }
QScrollBar::add-page, QScrollBar::sub-page { background: none; }

QMessageBox, QFileDialog { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #d8dee8, stop:1 #a0aab8); }
QMessageBox QPushButton, QFileDialog QPushButton {
    min-width: 70px; padding: 4px 12px; border-radius: 12px; border: 1px solid #6a7486;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f4f6fa, stop:1 #c3cad6);
}
)");
}

} // namespace Theme
