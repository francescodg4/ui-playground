#include "ThemeRegistry.hpp"
#include "WidgetGallery.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QElapsedTimer>
#include <QTimer>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("sparapinz"));
    QApplication::setApplicationName(QStringLiteral("WidgetGallery"));
    QApplication::setApplicationVersion(QStringLiteral(GALLERY_VERSION));

    QStringList ids;
    for (const ThemeEntry& theme : Themes::all()) {
        ids << theme.id;
    }

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("The standard Qt widgets in each interface design"));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption themeOption("theme", QStringLiteral("Interface to use: %1 (default: the last one selected).").arg(ids.join(QStringLiteral(", "))), "id");
    const QCommandLineOption darkOption("dark", "Use the dark mode of the designs that have one.");
    const QCommandLineOption screenshotOption("screenshot", "Save the window as gallery-<theme>.png in <dir> and quit; --theme all saves every interface.", "dir");
    parser.addOptions({ themeOption, darkOption, screenshotOption });
    parser.process(app);

    const QString requested = parser.value(themeOption);
    if (!requested.isEmpty() && requested != QLatin1String("all") && !Themes::find(requested)) {
        qWarning("Unknown theme '%s' (available: %s)", qPrintable(requested), qPrintable(ids.join(QStringLiteral(", "))));
        return 1;
    }
    const QStringList themes = requested == QLatin1String("all") ? ids : QStringList { requested.isEmpty() ? Themes::saved() : requested };

    WidgetGallery gallery;
    if (parser.isSet(darkOption)) {
        gallery.setDark(true);
    }
    gallery.setTheme(themes.first());
    gallery.show();

    if (parser.isSet(screenshotOption)) {
        QTimer::singleShot(300, &gallery, [&] {
            const QDir dir(parser.value(screenshotOption));
            dir.mkpath(QStringLiteral("."));
            for (const QString& id : themes) {
                gallery.setTheme(id);
                QElapsedTimer settle; // let the restyled window run a few event loop turns, as it would live
                settle.start();
                while (settle.elapsed() < 200) {
                    QApplication::processEvents(QEventLoop::AllEvents, 20);
                }
                const bool dark = gallery.dark() && Themes::find(id)->darkStyle;
                gallery.grab().save(dir.filePath(QStringLiteral("gallery-%1%2.png").arg(id, dark ? QStringLiteral("-dark") : QString())));
            }
            QApplication::quit();
        });
    }
    return app.exec();
}
