#include "bluetoothworker.h"

#include <QDebug>
#include <QProcess>
#include <QThread>

BluetoothWorker::BluetoothWorker(QQmlContext *context, QObject *parent)
    : QObject(parent), bluetooth(new Bluetooth(true, context, true, this))
{
    qDebug() << "[BluetoothWorker] created in thread:" << QThread::currentThread();
    QObject::connect(bluetooth, &Bluetooth::powerChanged,
            this, &BluetoothWorker::powerChanged, Qt::QueuedConnection);
    QObject::connect(bluetooth, &Bluetooth::requestConfirmationEvent,
            this, &BluetoothWorker::requestConfirmationEvent, Qt::QueuedConnection);
    QObject::connect(this, &BluetoothWorker::connect,
            bluetooth, qOverload<QString>(&Bluetooth::connect), Qt::QueuedConnection);
    QObject::connect(this, &BluetoothWorker::disconnect,
            bluetooth, qOverload<QString>(&Bluetooth::disconnect), Qt::QueuedConnection);
    QObject::connect(this, &BluetoothWorker::pair,
            bluetooth, qOverload<QString>(&Bluetooth::pair), Qt::QueuedConnection);
}

BluetoothWorker::~BluetoothWorker()
{
    qDebug() << "[BluetoothWorker] destroyed";
}

void BluetoothWorker::pair(QString id){
    qDebug() << "[BluetoothWorker] pair() called";
}

void BluetoothWorker::connect(QString id){
    qDebug() << "[BluetoothWorker] connect() called";
}

void BluetoothWorker::disconnect(QString id){
    qDebug() << "[BluetoothWorker] disconnect() called";
}

void BluetoothWorker::setPower(bool state){
    bluetooth->setPower(state);
    emit powerChanged(state);
}

void BluetoothWorker::send_confirmation(int pincode){
    bluetooth->send_confirmation(pincode);
}

void BluetoothWorker::setDiscoverable(bool state){
    bluetooth->setDiscoverable(state);
}

void BluetoothWorker::startDiscovery()
{
    qDebug() << "[BluetoothWorker] startDiscovery() thread =" << QThread::currentThread();

    //bool ok = QProcess::startDetached("bluetoothctl", QStringList() << "scan" << "on");

    bluetooth->start_discovery();

    emit discoveryStarted();
}

void BluetoothWorker::stopDiscovery()
{
    qDebug() << "[BluetoothWorker] stopDiscovery() thread =" << QThread::currentThread();

    // bool ok = QProcess::startDetached("bluetoothctl", QStringList() << "scan" << "off");

    // if (!ok) {
    //     emit errorOccurred("Failed to stop bluetooth discovery");
    //     return;
    // }
    bluetooth->stop_discovery();

    emit discoveryStopped();
}

void BluetoothWorker::start(){

        bluetooth->start();
}