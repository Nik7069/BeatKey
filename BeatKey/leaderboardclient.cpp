#include "leaderboardclient.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include <algorithm>

namespace {
const QString LeaderboardBaseUrl = QStringLiteral("https://arcadeleaderboard-5c0d2-default-rtdb.europe-west1.firebasedatabase.app");
}

LeaderboardClient::LeaderboardClient(QObject *parent)
    : QObject(parent)
{
}

void LeaderboardClient::fetch(const QString &mapId)
{
    if (LeaderboardBaseUrl.contains("YOUR-PROJECT")) {
        emit requestFailed("Укажите адрес Firebase Realtime Database в leaderboardclient.cpp.");
        return;
    }

    QNetworkReply *reply = network.get(QNetworkRequest(mapUrl(mapId)));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit requestFailed("Не удалось загрузить таблицу лидеров: " + reply->errorString());
            return;
        }

        emit fetchFinished(parseEntries(reply->readAll()));
    });
}

void LeaderboardClient::submit(const QString &mapId, const QString &nickname, int score)
{
    if (LeaderboardBaseUrl.contains("YOUR-PROJECT")) {
        emit requestFailed("Укажите адрес Firebase Realtime Database в leaderboardclient.cpp.");
        return;
    }

    QNetworkReply *getReply = network.get(QNetworkRequest(playerUrl(mapId, nickname)));
    connect(getReply, &QNetworkReply::finished, this, [this, getReply, mapId, nickname, score]() {
        getReply->deleteLater();

        if (getReply->error() != QNetworkReply::NoError) {
            emit requestFailed("Не удалось проверить никнейм: " + getReply->errorString());
            return;
        }

        const QByteArray currentData = getReply->readAll();
        const QJsonDocument currentDoc = QJsonDocument::fromJson(currentData);
        if (currentDoc.isObject()) {
            const int oldScore = currentDoc.object().value("score").toInt();
            if (score <= oldScore) {
                emit submitFinished(false, QString("Никнейм уже есть. Его рекорд: %1 очков.").arg(oldScore));
                return;
            }
        }

        QJsonObject record;
        record.insert("nickname", nickname);
        record.insert("score", score);
        record.insert("updatedAt", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

        QNetworkRequest request(playerUrl(mapId, nickname));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply *putReply = network.put(request, QJsonDocument(record).toJson(QJsonDocument::Compact));
        connect(putReply, &QNetworkReply::finished, this, [this, putReply]() {
            putReply->deleteLater();

            if (putReply->error() != QNetworkReply::NoError) {
                emit requestFailed("Не удалось сохранить результат: " + putReply->errorString());
                return;
            }

            emit submitFinished(true, "Результат сохранен в таблице лидеров.");
        });
    });
}

QUrl LeaderboardClient::mapUrl(const QString &mapId) const
{
    return QUrl(LeaderboardBaseUrl + "/leaderboards/" + firebaseKey(mapId) + ".json");
}

QUrl LeaderboardClient::playerUrl(const QString &mapId, const QString &nickname) const
{
    return QUrl(LeaderboardBaseUrl + "/leaderboards/" + firebaseKey(mapId) + "/" + firebaseKey(nickname) + ".json");
}

QString LeaderboardClient::firebaseKey(QString value) const
{
    value = value.trimmed();
    if (value.isEmpty())
        value = "unknown";

    for (QChar &ch : value) {
        if (ch == '.' || ch == '#' || ch == '$' || ch == '[' || ch == ']' || ch == '/')
            ch = '_';
    }

    return QString::fromLatin1(QUrl::toPercentEncoding(value));
}

QList<LeaderboardEntry> LeaderboardClient::parseEntries(const QByteArray &data) const
{
    QList<LeaderboardEntry> entries;
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
        return entries;

    const QJsonObject root = doc.object();
    for (auto it = root.constBegin(); it != root.constEnd(); ++it) {
        if (!it.value().isObject())
            continue;

        const QJsonObject item = it.value().toObject();
        LeaderboardEntry entry;
        entry.nickname = item.value("nickname").toString(it.key());
        entry.score = item.value("score").toInt();
        entry.updatedAt = QDateTime::fromString(item.value("updatedAt").toString(), Qt::ISODate);
        entries.append(entry);
    }

    std::sort(entries.begin(), entries.end(), [](const LeaderboardEntry &left, const LeaderboardEntry &right) {
        if (left.score == right.score)
            return left.updatedAt > right.updatedAt;
        return left.score > right.score;
    });

    return entries;
}
