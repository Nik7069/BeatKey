#ifndef LEADERBOARDCLIENT_H
#define LEADERBOARDCLIENT_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QNetworkAccessManager>
#include <QUrl>

struct LeaderboardEntry
{
    QString nickname;
    int score = 0;
    QDateTime updatedAt;
};

class LeaderboardClient : public QObject
{
    Q_OBJECT

public:
    explicit LeaderboardClient(QObject *parent = nullptr);

    void fetch(const QString &mapId);
    void submit(const QString &mapId, const QString &nickname, int score);

signals:
    void fetchFinished(const QList<LeaderboardEntry> &entries);
    void submitFinished(bool saved, const QString &message);
    void requestFailed(const QString &message);

private:
    QUrl mapUrl(const QString &mapId) const;
    QUrl playerUrl(const QString &mapId, const QString &nickname) const;
    QString firebaseKey(QString value) const;
    QList<LeaderboardEntry> parseEntries(const QByteArray &data) const;

    QNetworkAccessManager network;
};

#endif // LEADERBOARDCLIENT_H
