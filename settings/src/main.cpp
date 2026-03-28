#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

// #include <bluetooth.h>
// #include <network.h>

#include "bluetoothmanager.h"

int main(int argc, char *argv[]) {
	setenv("QT_QUICK_CONTROLS_STYLE", "AGL", 1);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    
    QQmlContext *context = engine.rootContext();
    // context->setContextProperty("bluetooth", new Bluetooth(true, context));
    // Network *network = new Network(true, context);
    // network->power(true, QString("bluetooth"));
    // context->setContextProperty("network", network);    

BluetoothManager bluetoothmanager(context);

    context->setContextProperty("bluetooth", &bluetoothmanager);

    engine.load(QUrl(QStringLiteral("qrc:/Settings.qml")));
    if (engine.rootObjects().isEmpty()) return -1;
    return app.exec();
}