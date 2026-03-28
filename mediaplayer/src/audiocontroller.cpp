#include "audiocontroller.h"

#include <QProcess>
#include <QRegularExpression>
#include <QtGlobal>
#include <QDebug>
#include <cmath>

AudioController::AudioController(QObject *parent)
    : QObject(parent)
{
    refreshTarget();
}

double AudioController::volume() const
{
    return m_volume;
}

QString AudioController::appNameHint() const
{
    return m_appNameHint;
}

void AudioController::setAppNameHint(const QString &name)
{
    if (m_appNameHint == name)
        return;

    m_appNameHint = name;
    emit appNameHintChanged();

    refreshTarget();
}

QString AudioController::runCommand(const QString &program, const QStringList &args) const
{
    QProcess process;
    process.start(program, args);
    process.waitForFinished(3000);

    QString out = QString::fromLocal8Bit(process.readAllStandardOutput());
    QString err = QString::fromLocal8Bit(process.readAllStandardError());

    if (!err.trimmed().isEmpty())
        qWarning() << program << args << err;

    return out;
}

QString AudioController::findStreamIdByHint() const
{
    const QString status = runCommand("wpctl", {"status"});
    if (status.isEmpty())
        return QString();

    const QStringList lines = status.split('\n');

    bool inStreams = false;
    QRegularExpression streamTopRegex("^\\s{6,}(\\d+)\\.\\s+(.+)$");
    QRegularExpression childRegex("^\\s{10,}(\\d+)\\.\\s+(.+)$");

    QString firstTopLevelStreamId;

    for (const QString &line : lines) {
        if (line.contains("└─ Streams:")) {
            inStreams = true;
            continue;
        }

        if (!inStreams)
            continue;

        if (line.startsWith("Video") || line.startsWith("Settings"))
            break;

        // Bỏ qua dòng con như 176, 177
        if (childRegex.match(line).hasMatch())
            continue;

        QRegularExpressionMatch match = streamTopRegex.match(line);
        if (!match.hasMatch())
            continue;

        const QString id = match.captured(1);
        const QString name = match.captured(2).trimmed();

        if (firstTopLevelStreamId.isEmpty())
            firstTopLevelStreamId = id;

        if (name.toLower().contains(m_appNameHint.toLower()))
            return id;
    }

    // không match tên thì lấy stream đầu tiên
    return firstTopLevelStreamId;
}

bool AudioController::inspectMatches(const QString &id) const
{
    const QString inspect = runCommand("wpctl", {"inspect", id});
    if (inspect.isEmpty())
        return false;

    const QString haystack = inspect.toLower();
    const QString hint = m_appNameHint.toLower();

    // Bạn có thể thêm điều kiện nếu app của bạn hiện ra bằng tên khác
    if (haystack.contains("application.name") && haystack.contains(hint))
        return true;

    if (haystack.contains("node.name") && haystack.contains(hint))
        return true;

    if (haystack.contains("media.name") && haystack.contains(hint))
        return true;

    return false;
}

QString AudioController::resolveTargetId()
{
    QString id = findStreamIdByHint();

    if (id.isEmpty()) {
        qWarning() << "Cannot find any stream, fallback to default sink";
        id = "@DEFAULT_AUDIO_SINK@";
    } else {
        qDebug() << "Resolved audio target =" << id;
    }

    m_targetId = id;
    emit targetChanged();
    return m_targetId;
}

void AudioController::refreshTarget()
{
    resolveTargetId();
}

void AudioController::setVolume(double value)
{
    m_volume = value;
    emit volumeChanged();

    if (m_targetId.isEmpty())
        resolveTargetId();

    const QString volStr = QString::number(value, 'f', 2);

    // nếu stream id cũ mất rồi thì tìm lại
    int rc = QProcess::execute("wpctl",
                               QStringList() << "set-volume"
                                             << m_targetId
                                             << volStr);

    if (rc != 0) {
        qWarning() << "set-volume failed for target" << m_targetId
                   << ", retry resolving target";
        resolveTargetId();

        QProcess::execute("wpctl",
                          QStringList() << "set-volume"
                                        << m_targetId
                                        << volStr);
    }
}

void AudioController::setVolumeMute(bool mute)
{
    if (m_targetId.isEmpty())
        resolveTargetId();

    int rc = QProcess::execute("wpctl",
                               QStringList() << "set-mute"
                                             << m_targetId
                                             << (mute ? "1" : "0"));

    if (rc != 0) {
        qWarning() << "set-mute failed for target" << m_targetId
                   << ", retry resolving target";
        resolveTargetId();

        QProcess::execute("wpctl",
                          QStringList() << "set-mute"
                                        << m_targetId
                                        << (mute ? "1" : "0"));
    }
}