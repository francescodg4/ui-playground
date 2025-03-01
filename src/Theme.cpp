#include "Theme.hpp"

namespace Theme {

QString styleSheet()
{
    return QStringLiteral(R"(
QWidget { color: #eef8ff; background: transparent; }
QToolTip { color: #eef8ff; background: #0b3f6e; border: 1px solid rgba(185, 230, 255, 140); padding: 3px 6px; }

QScrollArea, QListView, QTreeView { border: none; background: transparent; }

QScrollBar:vertical {
    width: 16px; margin: 0; border-radius: 3px;
    background: rgba(120, 190, 240, 30); border: 1px solid rgba(185, 230, 255, 56);
}
QScrollBar::handle:vertical { min-height: 36px; margin: 1px; border-radius: 3px; background: rgba(225, 243, 255, 215); }
QScrollBar:horizontal {
    height: 12px; margin: 0; border-radius: 3px;
    background: rgba(120, 190, 240, 30); border: 1px solid rgba(185, 230, 255, 56);
}
QScrollBar::handle:horizontal { min-width: 36px; margin: 1px; border-radius: 3px; background: rgba(225, 243, 255, 215); }
QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }
QScrollBar::add-page, QScrollBar::sub-page { background: none; }

QToolButton#round {
    border: 1.5px solid rgba(185, 230, 255, 140); border-radius: 10px;
    background: rgba(15, 70, 125, 115);
}
QToolButton#round:hover { background: rgba(50, 140, 220, 140); border-color: rgba(220, 245, 255, 220); }
QToolButton#round:checked { background: rgba(15, 70, 125, 115); }

QToolButton#logPlay {
    border: 1.5px solid rgba(94, 240, 180, 180); border-radius: 10px;
    background: rgba(10, 60, 90, 115);
}
QToolButton#logPlay:hover { background: rgba(94, 240, 180, 50); }
QToolButton#logPlay[playing="true"] { background: rgba(94, 240, 180, 80); border-color: #5ef0b4; }

QToolButton#nav {
    border: 1.5px solid rgba(185, 230, 255, 140); border-radius: 10px;
    background: rgba(15, 70, 125, 130);
}
QToolButton#nav:hover { background: rgba(50, 140, 220, 150); }
QToolButton#nav:disabled { border-color: rgba(185, 230, 255, 50); background: rgba(15, 70, 125, 40); }

QPushButton#chip {
    padding: 4px 12px 4px 8px; border-radius: 12px;
    border: 1.5px solid rgba(185, 230, 255, 140); background: rgba(15, 70, 125, 115);
}
QPushButton#chip:hover { background: rgba(50, 140, 220, 140); }

QLabel#dim { color: #a9cfe8; }
QLabel#day { color: #5ef0b4; font-weight: bold; }
QLabel#entryTitle { font-size: 20pt; }
QLabel#category {
    padding: 4px 8px; font-weight: bold;
    border: 1px solid rgba(185, 230, 255, 56);
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 rgba(160, 210, 245, 95), stop:1 rgba(160, 210, 245, 35));
}

QListWidget#gallery::item { border: 1px solid rgba(200, 235, 255, 64); padding: 0; margin: 0; }
QListWidget#gallery::item:hover { border: 2px solid rgba(220, 245, 255, 235); }
QListWidget#filmstrip::item { border: 1px solid rgba(200, 235, 255, 64); }
QListWidget#filmstrip::item:selected { border: 2px solid #ffffff; background: transparent; }

QMessageBox, QFileDialog { background: #0b3f6e; }
QMessageBox QPushButton, QFileDialog QPushButton {
    min-width: 70px; padding: 4px 10px; border-radius: 6px;
    border: 1px solid rgba(185, 230, 255, 140); background: rgba(40, 120, 200, 160);
}
)");
}

QFont titleFont()
{
    QFont font;
    font.setPointSizeF(8.5);
    font.setBold(true);
    font.setCapitalization(QFont::AllUppercase);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 1.0);
    return font;
}

} // namespace Theme
