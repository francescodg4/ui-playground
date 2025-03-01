#include "Theme.hpp"

#include <QApplication>
#include <QWidget>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("PDA"));

    QApplication::setStyle(QStringLiteral("Fusion"));
    app.setStyleSheet(Theme::styleSheet());
    QFont font = QApplication::font();
    font.setPointSizeF(10);
    QApplication::setFont(font);

    QWidget window;
    window.setWindowTitle(QStringLiteral("PDA"));
    window.resize(1100, 640);
    window.show();
    return app.exec();
}
