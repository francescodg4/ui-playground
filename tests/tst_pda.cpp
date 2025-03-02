#include "Icons.hpp"
#include "PhotoLibrary.hpp"

#include <QImage>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

class TestPda : public QObject {
    Q_OBJECT

private:
    static void writeImage(const QString& path, const QDateTime& time = {})
    {
        QImage image(16, 9, QImage::Format_RGB32);
        image.fill(Qt::blue);
        QVERIFY(image.save(path));
        if (time.isValid()) {
            QFile file(path);
            QVERIFY(file.open(QIODevice::ReadWrite));
            QVERIFY(file.setFileTime(time, QFileDevice::FileModificationTime));
        }
    }

private slots:
    void svgPathAbsolute()
    {
        const QPainterPath path = Icons::svgPath(u"M0 0H10V10H0Z");
        QCOMPARE(path.boundingRect(), QRectF(0, 0, 10, 10));
    }

    void svgPathRelativeAndImplicit()
    {
        // "m" followed by extra pairs means relative linetos
        const QPainterPath path = Icons::svgPath(u"m2 3 4 0 0 5z");
        QCOMPARE(path.boundingRect(), QRectF(2, 3, 4, 5));
        QCOMPARE(path.elementCount(), 4);
    }

    void svgPathCompactNumbers()
    {
        // numbers glued together as in minified SVG: ".6-5" is 0.6 then -5
        const QPainterPath path = Icons::svgPath(u"M10 10l.6-5");
        QCOMPARE(path.currentPosition(), QPointF(10.6, 5));
    }

    void svgPathSmoothCurve()
    {
        const QPainterPath path = Icons::svgPath(u"M0 0C0 10 10 10 10 0S20-10 20 0");
        QCOMPARE(path.currentPosition(), QPointF(20, 0));
        QVERIFY(path.boundingRect().top() < -5); // the reflected control point bends the 2nd half upwards
    }

    void iconsRender()
    {
        const QImage image = Icons::pixmap(Icon::Titanium, QSize(32, 32)).toImage();
        bool painted = false;
        for (int y = 0; y < image.height() && !painted; ++y) {
            for (int x = 0; x < image.width() && !painted; ++x) {
                painted = qAlpha(image.pixel(x, y)) > 0;
            }
        }
        QVERIFY(painted);
    }

    void displayName()
    {
        QCOMPARE(PhotoLibrary::displayName("kelp_forest.png"), QString("Kelp forest"));
        QCOMPARE(PhotoLibrary::displayName("/a/b/01-reef-shot.jpg"), QString("01 reef shot"));
    }

    void listsOnlyImagesOldestFirst()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QDateTime now = QDateTime::currentDateTime();
        writeImage(dir.filePath("b.png"), now.addSecs(-100));
        writeImage(dir.filePath("a.png"), now.addSecs(-50));
        writeImage(dir.filePath("c.jpg"), now.addSecs(-200));
        QFile notes(dir.filePath("notes.txt"));
        QVERIFY(notes.open(QIODevice::WriteOnly));

        PhotoLibrary library(dir.path());
        QCOMPARE(library.photos().size(), 3);
        QCOMPARE(QFileInfo(library.photos()[0].path).fileName(), QString("c.jpg"));
        QCOMPARE(QFileInfo(library.photos()[1].path).fileName(), QString("b.png"));
        QCOMPARE(QFileInfo(library.photos()[2].path).fileName(), QString("a.png"));
    }

    void importRenamesOnCollision()
    {
        QTemporaryDir source, target;
        writeImage(source.filePath("shot.png"));
        writeImage(target.filePath("shot.png"));

        PhotoLibrary library(target.path());
        QSignalSpy changed(&library, &PhotoLibrary::changed);
        const QStringList imported = library.import({ source.filePath("shot.png"), source.filePath("missing.png") });
        QCOMPARE(imported.size(), 1);
        QCOMPARE(QFileInfo(imported[0]).fileName(), QString("shot-2.png"));
        QCOMPARE(library.photos().size(), 2);
        QCOMPARE(changed.count(), 1);
    }

    void removeDeletesFile()
    {
        QTemporaryDir dir;
        writeImage(dir.filePath("one.png"));
        PhotoLibrary library(dir.path());
        const QString path = library.photos()[0].path;
        QVERIFY(library.remove(0));
        QVERIFY(!QFile::exists(path));
        QVERIFY(library.photos().isEmpty());
        QVERIFY(!library.remove(0));
    }

    void picksUpNewAcquisitions()
    {
        QTemporaryDir dir;
        PhotoLibrary library(dir.path());
        QSignalSpy changed(&library, &PhotoLibrary::changed);
        writeImage(dir.filePath("new.png"));
        QVERIFY(changed.wait(3000));
        QCOMPARE(library.photos().size(), 1);
    }
};

QTEST_MAIN(TestPda)
#include "tst_pda.moc"
