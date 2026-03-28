#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QObject>
#include <QThread>
#include <QQmlContext>


class BluetoothWorker;

class BluetoothManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool discovering READ discovering NOTIFY discoveringChanged)
    Q_PROPERTY(bool power READ power WRITE setPower NOTIFY powerChanged)
    Q_PROPERTY(bool discoverable READ discoverable WRITE setDiscoverable NOTIFY discoverableChanged)

public:
    explicit BluetoothManager(QQmlContext *context, QObject *parent = nullptr);
    ~BluetoothManager();

    bool discovering() const;

    Q_INVOKABLE void start_discovery();
    Q_INVOKABLE void stop_discovery();
    Q_INVOKABLE void start();

    Q_INVOKABLE void pair(QString id);
    Q_INVOKABLE void connect(QString id);
    Q_INVOKABLE void disconnect(QString id);

    Q_INVOKABLE void setPower(bool state);
	Q_INVOKABLE void send_confirmation(int pincode);


    bool power() const{return m_power;};
    bool discoverable() const { return m_discoverable; };

signals:
    void requestStartDiscovery();
    void requestStopDiscovery();
    void requestStart();
    void powerChanged(bool state);

    void discoveringChanged();
    void discoverableChanged(bool state);
    void errorOccurred(const QString &message);
    void requestConfirmationEvent(QString pincode);

private slots:
    void onDiscoveryStarted();
    void onDiscoveryStopped();
    void onWorkerError(const QString &message);
    void setDiscoverable(bool state);

private:
    QThread *m_workerThread;
    BluetoothWorker *m_worker;
    bool m_discovering;
    bool m_power;
	bool m_discoverable;
};

#endif // BLUETOOTHMANAGER_H