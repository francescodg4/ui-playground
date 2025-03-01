#include "PhotoLibrary.hpp"

#include <QDir>
#include <QFileInfo>

#include <algorithm>

PhotoLibrary::PhotoLibrary(const QString& directory, QObject* parent)
    : QObject(parent)
    , m_directory(QDir(directory).absolutePath())
{
    QDir().mkpath(m_directory);
    m_watcher.addPath(m_directory);

    // a camera writing a file triggers several notifications: coalesce them
    m_debounce.setSingleShot(true);
    m_debounce.setInterval(250);
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, &m_debounce, qOverload<>(&QTimer::start));
    connect(&m_debounce, &QTimer::timeout, this, &PhotoLibrary::refresh);

    refresh();
}

bool PhotoLibrary::isImageFile(const QString& fileName)
{
    static const QStringList suffixes = { "png", "jpg", "jpeg", "bmp", "gif" };
    return suffixes.contains(QFileInfo(fileName).suffix().toLower());
}

QString PhotoLibrary::displayName(const QString& fileName)
{
    QString name = QFileInfo(fileName).completeBaseName();
    name.replace(QLatin1Char('_'), QLatin1Char(' ')).replace(QLatin1Char('-'), QLatin1Char(' '));
    name = name.simplified();
    if (!name.isEmpty()) {
        name[0] = name[0].toUpper();
    }
    return name;
}

void PhotoLibrary::refresh()
{
    QList<Photo> photos;
    const QFileInfoList files = QDir(m_directory).entryInfoList(QDir::Files | QDir::Readable, QDir::Name);
    for (const QFileInfo& info : files) {
        if (isImageFile(info.fileName())) {
            photos.append({ info.absoluteFilePath(), displayName(info.fileName()), info.lastModified() });
        }
    }
    std::stable_sort(photos.begin(), photos.end(), [](const Photo& a, const Photo& b) { return a.acquired < b.acquired; });

    const auto samePhoto = [](const Photo& a, const Photo& b) { return a.path == b.path && a.acquired == b.acquired; };
    if (!std::equal(photos.begin(), photos.end(), m_photos.begin(), m_photos.end(), samePhoto)) {
        m_photos = photos;
        emit changed();
    }
}

QStringList PhotoLibrary::import(const QStringList& files)
{
    QStringList imported;
    const QDir dir(m_directory);
    for (const QString& file : files) {
        const QFileInfo info(file);
        if (!info.isFile() || !isImageFile(file)) {
            continue;
        }
        QString target = dir.filePath(info.fileName());
        for (int n = 2; QFileInfo::exists(target); ++n) {
            target = dir.filePath(QStringLiteral("%1-%2.%3").arg(info.completeBaseName()).arg(n).arg(info.suffix()));
        }
        if (QFile::copy(file, target)) {
            imported.append(target);
        }
    }
    if (!imported.isEmpty()) {
        refresh();
    }
    return imported;
}

bool PhotoLibrary::remove(int index)
{
    if (index < 0 || index >= m_photos.size() || !QFile::remove(m_photos[index].path)) {
        return false;
    }
    refresh();
    return true;
}
