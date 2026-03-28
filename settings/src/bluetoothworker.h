#ifndef BLUETOOTHWORKER_H
#define BLUETOOTHWORKER_H

#include <QObject>
#include <QQmlContext>
#include <bluetooth.h>

class BluetoothWorker : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothWorker(QQmlContext *context, QObject *parent = nullptr);
    ~BluetoothWorker();

    Bluetooth *bluetooth;

public slots:
    void startDiscovery();
    void stopDiscovery();
    void start();

    void pair(QString id);
    void connect(QString id);
    void disconnect(QString id);

    void setPower(bool state);
    void setDiscoverable(bool state);
	void send_confirmation(int pincode);


signals:
    void discoveryStarted();
    void discoveryStopped();
    void powerChanged(bool state);
    void errorOccurred(const QString &message);
    void requestConfirmationEvent(QString pincode);
};

#endif // BLUETOOTHWORKER_H