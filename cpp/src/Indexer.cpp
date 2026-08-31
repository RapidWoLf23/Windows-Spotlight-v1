#include "Indexer.h"

#include <QDataStream>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSaveFile>

#include <algorithm>

IndexWorker::IndexWorker(QObject *parent)
    : QObject(parent)
{
}

void IndexWorker::requestStop()
{
    m_stopRequested.store(
        true,
        std::memory_order_relaxed
    );
}

qint64 IndexWorker::estimateEntries(
    const QStringList &roots
) const
{
    qint64 total = 0;

    for (const QString &root : roots)
    {
        if (m_stopRequested.load(
                std::memory_order_relaxed))
        {
            return total;
        }

        QDirIterator iterator(
            root,
            QDir::Dirs |
            QDir::Files |
            QDir::NoDotAndDotDot,
            QDirIterator::Subdirectories
        );

        while (iterator.hasNext())
        {
            if (m_stopRequested.load(
                    std::memory_order_relaxed))
            {
                return total;
            }

            iterator.next();
            ++total;
        }
    }

    return qMax<qint64>(
        1,
        total
    );
}

void IndexWorker::buildIndex(
    const QStringList &roots,
    const QString &indexFile
)
{
    m_stopRequested.store(
        false,
        std::memory_order_relaxed
    );

    const qint64 totalEntries =
        estimateEntries(roots);

    if (m_stopRequested.load(
            std::memory_order_relaxed))
    {
        emit finished(
            false,
            0,
            "Indexing cancelled"
        );

        return;
    }

    QVector<IndexedApp> items;

    items.reserve(4096);

    qint64 visitedEntries = 0;

    if (!scanRoots(
            roots,
            items,
            totalEntries,
            visitedEntries))
    {
        emit finished(
            false,
            0,
            "Indexing cancelled"
        );

        return;
    }

    std::sort(
        items.begin(),
        items.end(),
        [](const IndexedApp &a,
           const IndexedApp &b)
        {
            if (a.searchName != b.searchName)
            {
                return a.searchName <
                       b.searchName;
            }

            return a.path < b.path;
        }
    );

    QVector<IndexedApp> unique;

    unique.reserve(
        items.size()
    );

    QString previousPath;

    for (const IndexedApp &item : items)
    {
        const QString normalized =
            QDir::fromNativeSeparators(
                item.path
            ).toCaseFolded();

        if (normalized == previousPath)
        {
            continue;
        }

        previousPath = normalized;

        unique.append(item);
    }

    items.swap(unique);

    if (!saveIndex(
            indexFile,
            items))
    {
        emit error(
            "The index could not be saved."
        );

        emit finished(
            false,
            items.size(),
            "Index save failed"
        );

        return;
    }

    emit progressChanged(
        100,
        items.size(),
        visitedEntries
    );

    emit indexReady(
        items
    );

    emit finished(
        true,
        items.size(),
        QString(
            "Index ready: %1 applications"
        ).arg(items.size())
    );
}

bool IndexWorker::scanRoots(
    const QStringList &roots,
    QVector<IndexedApp> &items,
    qint64 totalEntries,
    qint64 &visitedEntries
)
{
    for (const QString &root : roots)
    {
        if (m_stopRequested.load(
                std::memory_order_relaxed))
        {
            return false;
        }

        if (!QDir(root).exists())
        {
            continue;
        }

        QDirIterator iterator(
            root,
            QStringList()
                << "*.lnk"
                << "*.url"
                << "*.exe",
            QDir::Files |
            QDir::NoSymLinks,
            QDirIterator::Subdirectories
        );

        while (iterator.hasNext())
        {
            if (m_stopRequested.load(
                    std::memory_order_relaxed))
            {
                return false;
            }

            const QString path =
                iterator.next();

            ++visitedEntries;

            const QFileInfo info(path);

            if (!info.exists() ||
                !info.isFile())
            {
                continue;
            }

            const QString name =
                info.completeBaseName()
                    .trimmed();

            if (name.isEmpty())
            {
                continue;
            }

            IndexedApp item;

            item.name = name;

            item.searchName =
                name.toCaseFolded();

            item.path =
                QDir::fromNativeSeparators(
                    info.absoluteFilePath()
                );

            items.append(item);

            if ((visitedEntries & 127) == 0)
            {
                const int percent =
                    qBound(
                        1,
                        static_cast<int>(
                            (
                                visitedEntries *
                                100
                            ) /
                            qMax<qint64>(
                                1,
                                totalEntries
                            )
                        ),
                        99
                    );

                emit progressChanged(
                    percent,
                    items.size(),
                    visitedEntries
                );
            }
        }
    }

    return true;
}

bool IndexWorker::saveIndex(
    const QString &indexFile,
    const QVector<IndexedApp> &items
)
{
    const QFileInfo info(
        indexFile
    );

    QDir directory(
        info.absolutePath()
    );

    if (!directory.exists())
    {
        if (!directory.mkpath("."))
        {
            return false;
        }
    }

    QSaveFile file(
        indexFile
    );

    if (!file.open(
            QIODevice::WriteOnly))
    {
        return false;
    }

    QDataStream stream(
        &file
    );

    stream.setVersion(
        QDataStream::Qt_6_5
    );

    stream << quint32(0x57535054);
    stream << quint32(1);
    stream << quint32(items.size());

    for (const IndexedApp &item : items)
    {
        stream
            << item.name
            << item.searchName
            << item.path;

        if (stream.status() !=
            QDataStream::Ok)
        {
            return false;
        }
    }

    return file.commit();
}