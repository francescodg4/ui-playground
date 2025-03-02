#include "PdaWindow.hpp"
#include "PhotoLibrary.hpp"
#include "Theme.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QTimer>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("PDA"));
    QApplication::setApplicationVersion(QStringLiteral(PDA_VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Holographic PDA interface demo"));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption photosOption("photos", "Folder with the acquired images.", "dir", QStringLiteral(PDA_DEFAULT_PHOTO_DIR));
    const QCommandLineOption pageOption("page", "Page shown at start (1-6).", "n", "1");
    const QCommandLineOption fullScreenOption("fullscreen", "Run full screen (F11 toggles).");
    const QCommandLineOption screenshotOption("screenshot", "Save every page as page-N.png in <dir> and quit.", "dir");
    parser.addOptions({ photosOption, pageOption, fullScreenOption, screenshotOption });
    parser.process(app);

    QApplication::setStyle(QStringLiteral("Fusion"));
    app.setStyleSheet(Theme::styleSheet());
    QFont font = QApplication::font();
    font.setPointSizeF(10);
    QApplication::setFont(font);

    PhotoLibrary photos(parser.value(photosOption));
    PdaWindow window(&photos);
    window.resize(1100, 640);
    window.setPage(parser.value(pageOption).toInt() - 1);

    if (parser.isSet(screenshotOption)) {
        const QDir dir(parser.value(screenshotOption));
        dir.mkpath(QStringLiteral("."));
        window.show();
        QTimer::singleShot(300, &window, [&] {
            for (int i = 0; i < window.pageCount(); ++i) {
                window.setPage(i);
                QApplication::processEvents();
                window.grab().save(dir.filePath(QStringLiteral("page-%1.png").arg(i + 1)));
            }
            QApplication::quit();
        });
        return app.exec();
    }

    if (parser.isSet(fullScreenOption)) {
        window.showFullScreen();
    } else {
        window.show();
    }
    return app.exec();
}
