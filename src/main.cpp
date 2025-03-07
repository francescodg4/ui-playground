#include "PdaWindow.hpp"
#include "pages/PhotoPage.hpp"
#include "PhotoLibrary.hpp"
#include "Theme.hpp"
#include "widgets/Metal.hpp"

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
    const QCommandLineOption reducedMotionOption("reduced-motion", "Freeze the marquee and the visualizer.");
    const QCommandLineOption screenshotOption("screenshot", "Save every page as page-N.png in <dir> and quit.", "dir");
    parser.addOptions({ photosOption, pageOption, fullScreenOption, reducedMotionOption, screenshotOption });
    parser.process(app);

    QApplication::setStyle(QStringLiteral("Fusion"));
    app.setStyleSheet(Theme::styleSheet());
    QApplication::setFont(Metal::uiFont(9));

    PhotoLibrary photos(parser.value(photosOption));
    PdaWindow window(&photos);
    window.resize(1100, 640);
    window.setAnimationsEnabled(!parser.isSet(reducedMotionOption) && !parser.isSet(screenshotOption));
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
            if (auto* photoPage = window.findChild<PhotoPage*>(); photoPage && !photos.photos().isEmpty()) {
                window.setPage(window.pageIndex(photoPage));
                photoPage->openPhoto(0);
                QApplication::processEvents();
                window.grab().save(dir.filePath(QStringLiteral("photo-viewer.png")));
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
