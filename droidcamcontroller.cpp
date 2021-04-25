#include "droidcamcontroller.h"

#include <QThread>
#include <QtConcurrent/QtConcurrent>

DroidcamController::DroidcamController()
{
    setupConfig();
    droidcam.setProcessChannelMode(QProcess::MergedChannels);

    // setup signal handlers
    connect(&adbSetupWatcher, &QFutureWatcher<AdbSetupStatus>::finished, this, &DroidcamController::adbSetupFinished);
    connect(&adbCloseWatcher, &QFutureWatcher<AdbSetupStatus>::finished, this, &DroidcamController::adbCloseFinished);
    connect(&droidcam, &QProcess::readyReadStandardOutput, this, &DroidcamController::dcProcessOutput);
    connect(&droidcam, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &DroidcamController::dcProcessFinished);
}

void DroidcamController::start()
{
    if(status == DroidcamControllerStatus::IDLE) {
        status = DroidcamControllerStatus::CONNECTING;
        adbSetupWatcher.setFuture(QtConcurrent::run(this, &DroidcamController::runAdbSetup));
    }
}

void DroidcamController::stop()
{
    if(status == DroidcamControllerStatus::IDLE)
        return;

    droidcam.kill();
    adbCloseWatcher.setFuture(QtConcurrent::run(this, &DroidcamController::runAdbClose));
    status = DroidcamControllerStatus::IDLE;
}

DroidcamControllerStatus DroidcamController::getStatus()
{
    return status;
}

void DroidcamController::setupConfig()
{
    QString val;
    QFile configFile(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/droidcam.json");
    configFile.open(QIODevice::ReadOnly | QIODevice::Text);
    val = configFile.readAll();
    configFile.close();

    QJsonDocument configDoc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject configObj = configDoc.object();

    ip = configObj["ip"].toString();
    dcExec = configObj["dcExec"].toString();
    dcPort = configObj["dcPort"].toString();
    adbExec = configObj["adbExec"].toString();
    adbPort = configObj["adbPort"].toString();
    deviceId = configObj["deviceId"].toString();

    adbSocket = ip + ":" + adbPort;
    dcPkgName = "com.dev47apps.droidcam";
}

void DroidcamController::adbSetupFinished()
{
    AdbSetupStatus s = adbSetupWatcher.result();
    if(s == AdbSetupStatus::SUCCESFUL) {
        droidcam.start("droidcam-cli", {"-v", ip, dcPort});
        status = DroidcamControllerStatus::CONNECTED;
        return;
    }

    status = DroidcamControllerStatus::IDLE;
}

void DroidcamController::adbCloseFinished()
{

}

void DroidcamController::dcProcessOutput()
{
    QString demo = droidcam.readAllStandardOutput();
    droidcam.readAll();
    if(demo.contains("Error: Connection reset!") ||
       demo.contains("Is the app running?") ||
       demo.contains("Error: Error sending request, DroidCam might be busy with another client.")) {
            // kill all instances
            #ifdef Q_OS_LINUX
                QProcess::execute("killall", {dcExec});
            #endif

            #ifdef Q_OS_WIN
                QProcess::execute("taskkill", {"/IM", dcExec, "/F", "/T"});
            #endif

            stop();
    }
}

void DroidcamController::dcProcessFinished(int , QProcess::ExitStatus )
{
    status = DroidcamControllerStatus::IDLE;
}

AdbSetupStatus DroidcamController::runAdbSetup()
{
    if(!getDevices().contains(adbSocket))
        if(!connectDevice())
            if(!setupConnection()) {
                return AdbSetupStatus::DEVICE_CONNECTION_FAILURE;
            }

    if(!startApp()) {
        return AdbSetupStatus::APP_START_FAILURE;
    }

    if(!waitForUnlock()) {
        return AdbSetupStatus::UNLOCK_TIMEOUT;
    }

    QThread::sleep(1);
    return AdbSetupStatus::SUCCESFUL;
}

AdbSetupStatus DroidcamController::runAdbClose()
{
    QThread::sleep(1);
    QProcess::execute(adbExec, {"-s", adbSocket, "shell", "am", "force-stop", dcPkgName});
    return AdbSetupStatus::SUCCESFUL;
}

QString DroidcamController::getDevices()
{
    QProcess listDevices;
    listDevices.start(adbExec, {"devices"});
    listDevices.waitForFinished();
    return listDevices.readAll();
}

bool DroidcamController::connectDevice()
{
    QProcess connectDevice;
    connectDevice.start(adbExec, {"connect", adbSocket});
    connectDevice.waitForFinished();
    QString connection = connectDevice.readAllStandardOutput();
    return !connection.contains("failed to connect to");
}

bool DroidcamController::setupConnection()
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

bool DroidcamController::startApp()
{
    QProcess launchApp;
    launchApp.start(adbExec, {"-s", adbSocket, "shell", "monkey", "-p", dcPkgName, "1"});
    launchApp.waitForFinished();
    QString result = launchApp.readAll();
    return result.contains("Events injected: 1");
}

bool DroidcamController::waitForUnlock()
{
    // light screen
    if(!checkUnlocked())
        QProcess::execute(adbExec, {"-s", adbSocket, "shell", "input", "keyevent", "26"});
    else
        QThread::sleep(1);

    // wait for screen unlock
    int ticks = 0;
    while(!checkUnlocked() && ticks <= 30) {
        QThread::sleep(1);
        ticks++;
    }

    return ticks <= 30;
}

bool DroidcamController::checkUnlocked()
{
    QProcess checkStatus;
    checkStatus.start(adbExec, {"-s", adbSocket, "shell", "dumpsys", "window"});
    checkStatus.waitForFinished();
    QString result = checkStatus.readAll();
    return result.contains("mDreamingLockscreen=false");
}

