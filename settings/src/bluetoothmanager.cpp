#include "bluetoothmanager.h"
#include "bluetoothworker.h"

#include <QDebug>

BluetoothManager::BluetoothManager(QQmlContext *context, QObject *parent)
    : QObject(parent)
    , m_workerThread(new QThread(this))
    , m_worker(new BluetoothWorker(context))
    , m_discovering(false)
    , m_power(false)
{
    m_worker->moveToThread(m_workerThread);

    QObject::connect(m_workerThread, &QThread::finished,
            m_worker, &QObject::deleteLater);

    QObject::connect(m_worker, &BluetoothWorker::powerChanged,
            this, &BluetoothManager::powerChanged, Qt::QueuedConnection);
    
    QObject::connect(m_worker, &BluetoothWorker::requestConfirmationEvent,
            this, &BluetoothManager::requestConfirmationEvent, Qt::QueuedConnection);
    
    QObject::connect(this, &BluetoothManager::pair,
            m_worker, &BluetoothWorker::pair, Qt::QueuedConnection);
    
    QObject::connect(this, &BluetoothManager::connect,
            m_worker, &BluetoothWorker::connect, Qt::QueuedConnection);
    
    QObject::connect(this, &BluetoothManager::setDiscoverable,
            m_worker, &BluetoothWorker::setDiscoverable, Qt::QueuedConnection);

    QObject::connect(this, &BluetoothManager::disconnect,
            m_worker, &BluetoothWorker::disconnect, Qt::QueuedConnection);

    QObject::connect(this, &BluetoothManager::setPower,
            m_worker, &BluetoothWorker::setPower, Qt::QueuedConnection);

    QObject::connect(this, &BluetoothManager::send_confirmation,
            m_worker, &BluetoothWorker::send_confirmation, Qt::QueuedConnection);

    QObject::connect(this, &BluetoothManager::requestStart,
            m_worker, &BluetoothWorker::start, Qt::QueuedConnection);

    QObject::connect(this, &BluetoothManager::requestStartDiscovery,
            m_worker, &BluetoothWorker::startDiscovery, Qt::QueuedConnection);

    QObject::connect(this, &BluetoothManager::requestStopDiscovery,
            m_worker, &BluetoothWorker::stopDiscovery, Qt::QueuedConnection);

    QObject::connect(m_worker, &BluetoothWorker::discoveryStarted,
            this, &BluetoothManager::onDiscoveryStarted, Qt::QueuedConnection);

    QObject::connect(m_worker, &BluetoothWorker::discoveryStopped,
            this, &BluetoothManager::onDiscoveryStopped, Qt::QueuedConnection);

    QObject::connect(m_worker, &BluetoothWorker::errorOccurred,
            this, &BluetoothManager::onWorkerError, Qt::QueuedConnection);

    m_workerThread->start();

    qDebug() << "[BluetoothManager] worker thread started";
}

BluetoothManager::~BluetoothManager()
{
    qDebug() << "[BluetoothManager] shutting down worker thread";

    m_workerThread->quit();
    m_workerThread->wait();
}

void BluetoothManager::start(){
    emit requestStart();
}

void BluetoothManager::pair(QString id){
    qDebug() << "[BluetoothManager] pair() called";
}

void BluetoothManager::connect(QString id){
    qDebug() << "[BluetoothManager] connect() called";
}

void BluetoothManager::disconnect(QString id){
    qDebug() << "[BluetoothManager] disconnect() called";
}

void BluetoothManager::setPower(bool state){
    qDebug() << "[BluetoothManager] setPower() called";
    if(m_power != state) m_power = state;
    emit powerChanged(state);
}

void BluetoothManager::send_confirmation(int pincode){
    qDebug() << "[BluetoothManager] send_confirmation() called";
}

void BluetoothManager::setDiscoverable(bool state){
    qDebug() << "[BluetoothManager] setDiscoverable() called";
    m_discoverable = state;
}

bool BluetoothManager::discovering() const
{
    return m_discovering;
}

void BluetoothManager::start_discovery()
{
    qDebug() << "[BluetoothManager] start_discovery() called";
    emit requestStartDiscovery();
}

void BluetoothManager::stop_discovery()
{
    qDebug() << "[BluetoothManager] stop_discovery() called";
    emit requestStopDiscovery();
}

void BluetoothManager::onDiscoveryStarted()
{
    qDebug() << "[BluetoothManager] onDiscoveryStarted()";
    if (m_discovering)
        return;

    m_discovering = true;
    emit discoveringChanged();
}

void BluetoothManager::onDiscoveryStopped()
{
    qDebug() << "[BluetoothManager] onDiscoveryStopped()";
    if (!m_discovering)
        return;

    m_discovering = false;
    emit discoveringChanged();
}

void BluetoothManager::onWorkerError(const QString &message)
{
    qDebug() << "[BluetoothManager] error:" << message;
    emit errorOccurred(message);
}