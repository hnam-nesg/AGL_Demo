#include "searchcontroller.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QtPositioning/QGeoCoordinate>
#include <algorithm>

SearchController::SearchController(QObject *parent)
    : QObject(parent)
{
    m_debounce.setInterval(200);      // 200ms cho cảm giác giống GG Maps
    m_debounce.setSingleShot(true);
    connect(&m_debounce, &QTimer::timeout,
            this, &SearchController::doSearch);
}

void SearchController::scheduleSearch(const QString &query,
                                      double currentLat,
                                      double currentLon)
{
    const QString trimmed = query.trimmed();
    m_pendingQuery = trimmed;
    m_pendingLat   = currentLat;
    m_pendingLon   = currentLon;

    if (trimmed.size() < 0) {
        m_results.clear();
        emit resultsChanged();
        return;
    }

    m_debounce.start();
}

void SearchController::doSearch()
{
    const QString q = m_pendingQuery;
    if (q.size() < 2)
        return;

    ++m_requestSeq;
    const int seq = m_requestSeq;

    const double lat = m_pendingLat;
    const double lon = m_pendingLon;

    // viewbox quanh vị trí hiện tại
    const double delta = 0.2;
    const double left   = lon - delta;
    const double right  = lon + delta;
    const double top    = lat + delta;
    const double bottom = lat - delta;

    QUrl url(QStringLiteral("https://nominatim.openstreetmap.org/search"));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("format"), QStringLiteral("json"));
    query.addQueryItem(QStringLiteral("addressdetails"), QStringLiteral("1"));
    query.addQueryItem(QStringLiteral("limit"), QStringLiteral("20"));
    query.addQueryItem(QStringLiteral("countrycodes"), QStringLiteral("vn"));
    query.addQueryItem(QStringLiteral("viewbox"),
                       QStringLiteral("%1,%2,%3,%4")
                           .arg(left).arg(top).arg(right).arg(bottom));
    query.addQueryItem(QStringLiteral("q"), q);
    url.setQuery(query);

    QNetworkRequest req(url);
    req.setRawHeader("Accept-Language", "vi");
    req.setRawHeader("User-Agent", "YourAppName/1.0"); // nên đổi tên app

    QNetworkReply *reply = m_nam.get(req);

    // gắn metadata vào reply để xử lý đúng request
    reply->setProperty("seq", seq);
    reply->setProperty("query", q);
    reply->setProperty("lat", lat);
    reply->setProperty("lon", lon);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        const int seq = reply->property("seq").toInt();
        if (seq < m_requestSeq) {
            // đã có request mới hơn → bỏ
            return;
        }

        if (reply->error() != QNetworkReply::NoError)
            return;

        const QByteArray data = reply->readAll();
        const QJsonDocument doc = QJsonDocument::fromJson(data);
        const QJsonArray arr = doc.array();

        const QString query = reply->property("query").toString();
        const double curLat = reply->property("lat").toDouble();
        const double curLon = reply->property("lon").toDouble();
        const QGeoCoordinate curCoord(curLat, curLon);

        struct Result {
            QString displayName;
            QString primary;
            QString secondary;
            double lat;
            double lon;
            double distance = 0.0;
            int tscore = 0;
        };

        QVector<Result> tmp;
        tmp.reserve(arr.size());

        for (const QJsonValue &v : arr) {
            const QJsonObject o = v.toObject();
            Result r;
            r.displayName = o.value("display_name").toString();
            r.lat = o.value("lat").toString().toDouble();
            r.lon = o.value("lon").toString().toDouble();

            QGeoCoordinate c(r.lat, r.lon);
            r.distance = curCoord.distanceTo(c); // mét
            r.tscore   = textScore(query, r.displayName);

            // tách primary / secondary cho giống GG Maps
            const QStringList parts = r.displayName.split(',');
            if (!parts.isEmpty()) {
                r.primary = parts.first().trimmed();
                r.secondary = parts.mid(1).join(", ").trimmed();
            } else {
                r.primary = r.displayName;
            }

            tmp.push_back(r);
        }

        // Ưu tiên: 1) text giống hơn, 2) trong đó gần hơn
        std::sort(tmp.begin(), tmp.end(), [](const Result &a, const Result &b) {
            if (a.tscore != b.tscore)
                return a.tscore > b.tscore;
            return a.distance < b.distance;
        });

        QVariantList out;
        out.reserve(tmp.size());
        for (const Result &r : tmp) {
            QVariantMap m;
            m.insert("display_name", r.displayName);
            m.insert("primary", r.primary);
            m.insert("secondary", r.secondary);
            m.insert("lat", r.lat);
            m.insert("lon", r.lon);
            m.insert("distance", r.distance);
            m.insert("tscore", r.tscore);
            out.push_back(m);
        }

        m_results = out;
        emit resultsChanged();
    });
}

int SearchController::textScore(const QString &query,
                                const QString &name) const
{
    const QString q = query.toLower().trimmed();
    const QString n = name.toLower().trimmed();

    if (n == q)
        return 4;
    if (n.startsWith(q))
        return 3;

    const QStringList qWords = q.split(QRegularExpression("\\s+"),
                                       Qt::SkipEmptyParts);
    int matched = 0;
    for (const QString &w : qWords) {
        if (n.contains(w))
            ++matched;
    }

    if (matched == qWords.size() && matched > 0)
        return 2;
    if (matched > 0)
        return 1;
    return 0;
}
