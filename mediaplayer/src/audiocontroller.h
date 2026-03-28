#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

#include <QObject>
#include <QString>

class AudioController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(QString appNameHint READ appNameHint WRITE setAppNameHint NOTIFY appNameHintChanged)

public:
    explicit AudioController(QObject *parent = nullptr);

    double volume() const;
    QString appNameHint() const;

    Q_INVOKABLE void setVolume(double value);
    Q_INVOKABLE void setVolumeMute(bool mute);
    Q_INVOKABLE void setAppNameHint(const QString &name);
    Q_INVOKABLE QString resolveTargetId();
    Q_INVOKABLE void refreshTarget();

signals:
    void volumeChanged();
    void appNameHintChanged();
    void targetChanged();

private:
    QString runCommand(const QString &program, const QStringList &args) const;
    QString findStreamIdByHint() const;
    bool inspectMatches(const QString &id) const;

private:
    double m_volume = 0.5;                  // 0.0 -> 1.0
    QString m_appNameHint = "mediaplayer";  // đổi nếu app name khác
    QString m_targetId;                     // stream id runtime
};

#endif // AUDIOCONTROLLER_H