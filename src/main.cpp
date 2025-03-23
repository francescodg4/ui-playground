#include "PdaShell.hpp"
#include "PhotoLibrary.hpp"
#include "ThemeController.hpp"
#include "ThemeRegistry.hpp"
#include "WidgetGallery.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QElapsedTimer>
#include <QTimer>

namespace {

/// Saves every page (and the photo viewer) of the current window as PNG files in @p dir.
void capture(PdaShell* window, const PhotoLibrary& photos, const QDir& dir)
{
    dir.mkpath(QStringLiteral("."));
    for (int i = 0; i < window->pageCount(); ++i) {
        window->setPage(i);
        QApplication::processEvents();
        window->grab().save(dir.filePath(QStringLiteral("page-%1.png").arg(i + 1)));
    }
    if (!photos.photos().isEmpty()) {
        window->showPhotoViewer();
        QApplication::processEvents();
        window->grab().save(dir.filePath(QStringLiteral("photo-viewer.png")));
    }
}

} // namespace

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("sparapinz"));
    QApplication::setApplicationName(QStringLiteral("PDA"));
    QApplication::setApplicationVersion(QStringLiteral(PDA_VERSION));

    QStringList ids;
    for (const ThemeEntry& theme : Themes::all()) {
        ids << theme.id;
    }

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("PDA interface demo"));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption photosOption("photos", "Folder with the acquired images.", "dir", QStringLiteral(PDA_DEFAULT_PHOTO_DIR));
    const QCommandLineOption themeOption("theme", QStringLiteral("Theme to use: %1 (default: the last one selected).").arg(ids.join(QStringLiteral(", "))), "id");
    const QCommandLineOption pageOption("page", "Page shown at start (1-6).", "n", "1");
    const QCommandLineOption fullScreenOption("fullscreen", "Run full screen (F11 toggles).");
    const QCommandLineOption reducedMotionOption("reduced-motion", "No animations (transitions, moving canvas, visualizer, pulsing).");
    const QCommandLineOption reducedTransparencyOption("reduced-transparency", "Solid surfaces instead of glass (Liquid Glass theme).");
    const QCommandLineOption galleryOption("gallery", "Open the widget gallery: the standard widgets in each interface, to choose one.");
    const QCommandLineOption screenshotOption("screenshot", "Save every page as page-N.png in <dir> and quit; with --theme all, one folder per theme. With --gallery, saves gallery-<theme>.png.", "dir");
    parser.addOptions({ photosOption, themeOption, pageOption, fullScreenOption, reducedMotionOption, reducedTransparencyOption, galleryOption, screenshotOption });
    parser.process(app);

    QApplication::setStyle(QStringLiteral("Fusion"));

    const QString requested = parser.value(themeOption);
    if (!requested.isEmpty() && requested != QLatin1String("all") && !Themes::find(requested)) {
        qWarning("Unknown theme '%s' (available: %s)", qPrintable(requested), qPrintable(ids.join(QStringLiteral(", "))));
        return 1;
    }

    PhotoLibrary photos(parser.value(photosOption));
    ThemeOptions options;
    options.reducedMotion = parser.isSet(reducedMotionOption);
    options.reducedTransparency = parser.isSet(reducedTransparencyOption);
    ThemeController controller(&photos, options);

    if (parser.isSet(galleryOption)) {
        const QStringList themes = requested == QLatin1String("all") ? ids : QStringList { requested.isEmpty() ? Themes::saved() : requested };
        controller.showGallery(themes.first());
        if (parser.isSet(screenshotOption)) {
            QTimer::singleShot(300, &controller, [&] {
                const QDir dir(parser.value(screenshotOption));
                dir.mkpath(QStringLiteral("."));
                for (const QString& id : themes) {
                    controller.gallery()->setTheme(id);
                    QElapsedTimer settle; // let the restyled window run a few event loop turns, as it would live
                    settle.start();
                    while (settle.elapsed() < 200) {
                        QApplication::processEvents(QEventLoop::AllEvents, 20);
                    }
                    controller.gallery()->grab().save(dir.filePath(QStringLiteral("gallery-%1.png").arg(id)));
                }
                QApplication::quit();
            });
        }
        return app.exec();
    }

    if (parser.isSet(screenshotOption)) {
        controller.setAnimationsEnabled(false);
        const QDir dir(parser.value(screenshotOption));
        const QStringList themes = requested == QLatin1String("all") ? ids : QStringList { requested.isEmpty() ? Themes::saved() : requested };
        controller.apply(themes.first());
        controller.window()->show();
        QTimer::singleShot(300, &controller, [&] {
            for (const QString& id : themes) {
                controller.apply(id);
                controller.window()->show();
                QApplication::processEvents();
                capture(controller.window(), photos, themes.size() > 1 ? QDir(dir.filePath(id)) : dir);
            }
            QApplication::quit();
        });
        return app.exec();
    }

    controller.apply(requested.isEmpty() || requested == QLatin1String("all") ? Themes::saved() : requested);
    controller.window()->setPage(parser.value(pageOption).toInt() - 1);
    if (parser.isSet(fullScreenOption)) {
        controller.window()->showFullScreen();
    } else {
        controller.window()->show();
    }
    return app.exec();
}
