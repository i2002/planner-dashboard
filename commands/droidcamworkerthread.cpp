#include "droidcamworkerthread.h"
#include <QProcess>
#include <QDebug>

DroidcamWorkerThread::DroidcamWorkerThread(DroidcamWorkerType type, QString ip, QString dcExec, QString dcPort, QString adbExec, QString adbPort, QString deviceId, QObject *parent) :
    QThread(parent),
    type{type},
    ip{ip},
    dcExec{dcExec},
    dcPort{dcPort},
    adbExec{adbExec},
    adbPort{adbPort},
    deviceId{deviceId}
{
    adbSocket = ip + ":" + adbPort;
    dcPkgName = "com.dev47apps.droidcam";
}

void DroidcamWorkerThread::run()
{
    if(!getDevices().contains(adbSocket))
        if(!connectDevice())
            if(!setupConnection()) {
                emit resultReady("failed to connect device");
                return;
            }

    if(type == DroidcamWorkerType::STOP_DC) {
        // kill client
        #ifdef Q_OS_LINUX
            QProcess::execute("kill", {dcExec});
        #endif

        #ifdef Q_OS_WIN
            QProcess::execute("taskkill", {"/IM", dcExec, "/F"});
        #endif

        // kill app
        sleep(1);
        QProcess::execute(adbExec, {"-s", adbSocket, "shell", "monkey", "-p", dcPkgName, "1"});
        return;
    }

    if(!startApp()) {
        emit resultReady("failed to start droidcam");
        return;
    }

    if(!waitForUnlock()) {
        emit resultReady("unlock timeout");
        return;
    }

    //QProcess droidcam;
    droidcam = new QProcess();
    connect(droidcam, &QProcess::readyRead, this, &DroidcamWorkerThread::demo);
//    connect(droidcam, &QProcess::readyReadStandardError, [&](){ QString s = droidcam->readAllStandardError(); });
//    connect(droidcam, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &QObject::deleteLater);
    droidcam->setProcessChannelMode(QProcess::MergedChannels);
    droidcam->start(dcExec, {"-v", ip, dcPort});
//    droidcam->start("ls", QStringList());
    QString helloo;
    droidcam->waitForStarted();
    //droidcam->waitForFinished();
//    while(true)
//        //helloo = droidcam->readLine();
//        sleep(10);

    //QProcess::startDetached(dcExec, {"-v", "-nocontrols", ip, dcPort});
    int a = 3;
    a++;
    emit resultReady("aa");
}

void DroidcamWorkerThread::demo()
{
    QString demo = droidcam->readAllStandardOutput();
    qDebug() << demo;
}

QString DroidcamWorkerThread::getDevices()
{
    QProcess listDevices;
    listDevices.start(adbExec, {"devices"});
    listDevices.waitForFinished();
    return listDevices.readAll();
}

bool DroidcamWorkerThread::connectDevice()
{
    QProcess connectDevice;
    connectDevice.start(adbExec, {"connect", adbSocket});
    connectDevice.waitForFinished();
    QString connection = connectDevice.readAllStandardOutput();
    emit message(connection);
    return !connection.contains("failed to connect to");
}

bool DroidcamWorkerThread::setupConnection()
{
    // check if device usb connected
    if(!getDevices().contains(deviceId))
        return false;

    // setup tcp connection
    QProcess setup;
    setup.start(adbExec, {"-s", deviceId, "tcpip", adbPort});
    setup.waitForFinished();
    return connectDevice();
}

bool DroidcamWorkerThread::startApp()
{
    QProcess launchApp;
    launchApp.start(adbExec, {"-s", adbSocket, "shell", "monkey", "-p", dcPkgName, "1"});
    launchApp.waitForFinished();
    QString result = launchApp.readAll();
    return result.contains("Events injected: 1");
}

bool DroidcamWorkerThread::waitForUnlock()
{
    // light screen
    if(!checkUnlocked())
        QProcess::execute(adbExec, {"-s", adbSocket, "shell", "input", "keyevent", "26"});
    else
        sleep(1);

    // wait for screen unlock
    int ticks = 0;
    while(!checkUnlocked() && ticks <= 30) {
        sleep(1);
        ticks++;
    }

    return ticks <= 30;
}

bool DroidcamWorkerThread::checkUnlocked()
{
    QProcess checkStatus;
    checkStatus.start(adbExec, {"-s", adbSocket, "shell", "dumpsys", "window"});
    checkStatus.waitForFinished();
    QString result = checkStatus.readAll();
    return result.contains("mDreamingLockscreen=false");
}
