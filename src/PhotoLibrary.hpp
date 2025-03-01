#pragma once

#include <QDateTime>
#include <QFileSystemWatcher>
#include <QObject>
#include <QTimer>

struct Photo {
    QString path;
    QString name; ///< human readable, derived from the file name
    QDateTime acquired;
};

/// Images acquired by the device: the files of one folder, oldest first.
/// The folder is watched, so photos saved by the acquisition software show up on their own.
class PhotoLibrary : public QObject {
    Q_OBJECT
public:
    explicit PhotoLibrary(const QString& directory, QObject* parent = nullptr);

    QString directory() const { return m_directory; }
    const QList<Photo>& photos() const { return m_photos; }

    /// Re-reads the folder; emits changed() when the list differs.
    void refresh();

    /// Copies images into the folder (renaming on collisions). Returns the new file paths.
    QStringList import(const QStringList& files);

    /// Deletes the photo at @p index from disk.
    bool remove(int index);

    static bool isImageFile(const QString& fileName);
    static QString displayName(const QString& fileName);

signals:
    void changed();

private:
    QString m_directory;
    QList<Photo> m_photos;
    QFileSystemWatcher m_watcher;
    QTimer m_debounce;
};
