#include "droidcamcontroller.h"

#include <QMessageBox>
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
    QPair<AdbSetupStatus, QString> res = adbSetupWatcher.result();
    switch (res.first)
    {
    case AdbSetupStatus::SUCCESFUL:
#ifdef Q_OS_LINUX
        droidcam.start(dcExec, {"-v", ip, dcPort});
#endif

#ifdef Q_OS_WIN
        droidcam.start(dcExec, {"-c", ip, dcPort, "-video", "-tray"});
#endif
        break;

    case AdbSetupStatus::DEVICE_OFFLINE:
        QMessageBox::information(nullptr, "Droidcam setup failure", "Device offline, connect to WiFi\n" + res.second);
        break;

    case AdbSetupStatus::DEVICE_UNSET:
        QMessageBox::information(nullptr, "Droidcam setup failure", "Device unset, connect through USB to setup\n" + res.second);
        break;

    case AdbSetupStatus::DEVICE_UNAUTHORIZED:
        QMessageBox::information(nullptr, "Droidcam setup failure", "Please authorize connection" + res.second);
        break;

    case AdbSetupStatus::DEVICE_CONNECTION_FAILURE:
        QMessageBox::information(nullptr, "Droidcam setup failure", "Device connection failure\n" + res.second);
        break;

    case AdbSetupStatus::UNLOCK_TIMEOUT:
        QMessageBox::information(nullptr, "Droidcam setup failure", "Phone unlock timeout\n");
        break;

    case AdbSetupStatus::APP_START_FAILURE:
        QMessageBox::information(nullptr, "Droidcam setup failure", "Failed to start phone app\n");
        break;
    }

    status = res.first == AdbSetupStatus::SUCCESFUL ? DroidcamControllerStatus::CONNECTED : DroidcamControllerStatus::IDLE;
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

QPair<AdbSetupStatus, QString> DroidcamController::runAdbSetup()
{
    QString connection = connectDevice();
    if(connection.contains("failed to connect to") || connection == "") {
        if(connection.contains("No route to host") || connection == "") {
            return {AdbSetupStatus::DEVICE_OFFLINE, connection};
        }
        else if(connection.contains("Connection refused")) {
            if(!setupConnection()) {
                return {AdbSetupStatus::DEVICE_UNSET, connection};
            }
        }
        else if(connection.contains("failed to authenticate")) {
            return {AdbSetupStatus::DEVICE_UNAUTHORIZED, connection};
        }
        else {
            return {AdbSetupStatus::DEVICE_CONNECTION_FAILURE, connection};
        }

        QThread::sleep(5);
    }

    if(!startApp()) {
        return {AdbSetupStatus::APP_START_FAILURE, ""};
    }

    if(!waitForUnlock()) {
        return {AdbSetupStatus::UNLOCK_TIMEOUT, ""};
    }

    QThread::sleep(1);
    return {AdbSetupStatus::SUCCESFUL, ""};
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

QString DroidcamController::connectDevice()
{
    QProcess connectDevice;
    connectDevice.start(adbExec, {"connect", adbSocket});
    connectDevice.waitForFinished();
    QString connection = connectDevice.readAllStandardOutput();
    return connection;
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
    QThread::sleep(3);
    QString connection = connectDevice();
    return !connection.contains("failed to connect to");
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

