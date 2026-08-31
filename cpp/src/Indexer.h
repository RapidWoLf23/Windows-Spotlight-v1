#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>

#include <atomic>

struct IndexedApp
{
    QString name;
    QString searchName;
    QString path;
};

Q_DECLARE_METATYPE(IndexedApp)
Q_DECLARE_METATYPE(QVector<IndexedApp>)

class IndexWorker final : public QObject
{
    Q_OBJECT

public:
    explicit IndexWorker(QObject *parent = nullptr);

    void requestStop();

public slots:
    void buildIndex(
        const QStringList &roots,
        const QString &indexFile
    );

signals:
    void indexReady(
        const QVector<IndexedApp> &items
    );

    void progressChanged(
        int percent,
        qint64 indexed,
        qint64 discovered
    );

    void finished(
        bool success,
        qint64 itemCount,
        const QString &message
    );

    void error(
        const QString &message
    );

private:
    qint64 estimateEntries(
        const QStringList &roots
    ) const;

    bool scanRoots(
        const QStringList &roots,
        QVector<IndexedApp> &items,
        qint64 totalEntries,
        qint64 &visitedEntries
    );

    bool saveIndex(
        const QString &indexFile,
        const QVector<IndexedApp> &items
    );

    std::atomic_bool m_stopRequested{false};
};