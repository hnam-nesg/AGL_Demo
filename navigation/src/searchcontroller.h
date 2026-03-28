#pragma once

#include <QObject>
#include <QVariantList>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QVirtualKeyboardAbstractInputMethod>
#include <QVirtualKeyboardInputContext>
#include <QVirtualKeyboardInputEngine>

class SearchController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList results READ results NOTIFY resultsChanged)

public:
    explicit SearchController(QObject *parent = nullptr);

    // QML gọi hàm này mỗi khi text thay đổi
    Q_INVOKABLE void scheduleSearch(const QString &query,
                                    double currentLat,
                                    double currentLon);

    QVariantList results() const { return m_results; }

signals:
    void resultsChanged();

private slots:
    void doSearch();

private:
    int textScore(const QString &query, const QString &name) const;

    QNetworkAccessManager m_nam;
    QTimer m_debounce;

    int m_requestSeq = 0;         // id tăng dần cho mỗi request
    QString m_pendingQuery;
    double m_pendingLat = 0.0;
    double m_pendingLon = 0.0;

    QVariantList m_results;
};


