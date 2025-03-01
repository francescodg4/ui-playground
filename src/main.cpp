#include "PdaWindow.hpp"
#include "PhotoLibrary.hpp"
#include "Theme.hpp"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("PDA"));

    QApplication::setStyle(QStringLiteral("Fusion"));
    app.setStyleSheet(Theme::styleSheet());
    QFont font = QApplication::font();
    font.setPointSizeF(10);
    QApplication::setFont(font);

    PhotoLibrary photos(QStringLiteral(PDA_DEFAULT_PHOTO_DIR));
    PdaWindow window(&photos);
    window.resize(1100, 640);
    window.show();
    return app.exec();
}
