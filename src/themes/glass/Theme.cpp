#include "themes/glass/Theme.hpp"

namespace glass {

namespace Theme {

QString styleSheet()
{
    return QStringLiteral(R"(
QWidget { color: #eef6ff; background: transparent; }
QToolTip { color: #eef6ff; background: #102238; border: 1px solid rgba(255, 255, 255, 38); border-radius: 8px; padding: 4px 8px; }

QScrollArea, QListView, QTreeView { border: none; background: transparent; }

QScrollBar:vertical { width: 8px; margin: 4px 0; background: transparent; border: none; }
QScrollBar::handle:vertical { min-height: 32px; border-radius: 4px; background: rgba(255, 255, 255, 60); }
QScrollBar::handle:vertical:hover { background: rgba(255, 255, 255, 110); }
QScrollBar:horizontal { height: 8px; margin: 0 4px; background: transparent; border: none; }
QScrollBar::handle:horizontal { min-width: 32px; border-radius: 4px; background: rgba(255, 255, 255, 60); }
QScrollBar::handle:horizontal:hover { background: rgba(255, 255, 255, 110); }
QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }
QScrollBar::add-page, QScrollBar::sub-page { background: none; }

QLabel#dim { color: #a8c4de; }
QLabel#day { color: #5ef0b4; font-weight: bold; }
QLabel#cardTitle { font-weight: bold; }
QLabel#entryTitle { font-size: 20pt; }

QMessageBox, QFileDialog { background: #102238; }
QMessageBox QPushButton, QFileDialog QPushButton {
    min-width: 70px; padding: 6px 14px; border-radius: 16px;
    border: 1px solid rgba(255, 255, 255, 38); background: rgba(59, 155, 255, 140);
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

} // namespace glass
