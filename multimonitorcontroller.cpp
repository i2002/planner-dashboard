#include "multimonitorcontroller.h"

#include <QtConcurrent/QtConcurrent>
#include <QDebug>

MultimonitorController::MultimonitorController(QObject *parent) : QObject(parent)
{
    setupConfig();

    // setup signal handlers
    connect(&monitorSetupWatcher, &QFutureWatcher<void>::finished, this, &MultimonitorController::monitorSetupFinished);
    connect(&monitorDisableWatcher, &QFutureWatcher<void>::finished, this, &MultimonitorController::monitorDisableFinished);

    // setup process handlers
    connect(&server, &QProcess::readyRead, this, &MultimonitorController::serverProcessOutput);
    connect(&server, &QProcess::started, this, &MultimonitorController::serverProcessStarted);
    connect(&server, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &MultimonitorController::serverProcessFinished);
}


// ############## Helpers ########################
void MultimonitorController::setupConfig()
{
    QString val;
    QFile configFile(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/multimonitor.json");
    configFile.open(QIODevice::ReadOnly | QIODevice::Text);
    val = configFile.readAll();
    configFile.close();

    QJsonDocument configDoc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject configObj = configDoc.object();

    mainWidth = configObj["mainWidth"].toInt();
    mainHeight = configObj["mainHeight"].toInt();
    mainName = configObj["mainName"].toString();
    virtualWidth = configObj["virtualWidth"].toInt();
    virtualHeight = configObj["virtualHeight"].toInt();
    virtualName = configObj["virtualName"].toString();
    serverPassword = configObj["serverPassword"].toString();
}

void MultimonitorController::setPosition(MultiMonitorPosition p)
{
    pos = p;
    switch (pos)
    {
    case MultiMonitorPosition::RIGHT:
        relativePos = "--right-of";
        clipX = mainWidth;
        clipY = mainHeight - virtualHeight;
        break;

    case MultiMonitorPosition::LEFT:
        relativePos = "--left-of";
        clipX = 0;
        clipY = mainHeight - virtualHeight;
        break;
    }
}

void MultimonitorController::startServer()
{
    if(server.state() == QProcess::ProcessState::NotRunning) {
        QString clip = QString("%1x%2+%3+%4").arg(virtualWidth).arg(virtualHeight).arg(clipX).arg(clipY);
        server.start("x11vnc", {"-clip", clip, "-multiptr", "-noxdamage", "-noxcomposite", "-cursor", "arrow", "-repeat", "-passwd", serverPassword});
    }
}

void MultimonitorController::stopServer()
{
    // kill server
    if(server.state() == QProcess::ProcessState::Running) {
        server.terminate();
    }
}


// ################ Actions #######################
void MultimonitorController::setupMonitor(MultiMonitorPosition pos)
{
    if(status != MultimonitorControllerStatus::UNSET) {
        return;
    }

    status = MultimonitorControllerStatus::CONNECTING;
    setPosition(pos);
    monitorSetupWatcher.setFuture(QtConcurrent::run(this, &MultimonitorController::runMonitorSetup));
}

void MultimonitorController::restartServer()
{
    if(status != MultimonitorControllerStatus::UNSET && status != MultimonitorControllerStatus::IDLE && status != MultimonitorControllerStatus::CONNECTED) {
        return;
    }

    if(status == MultimonitorControllerStatus::IDLE) {
        status = MultimonitorControllerStatus::CONNECTING;
        runServerRestart();
    } else {
        status = MultimonitorControllerStatus::RESTARTING;
        stopServer();
    }
}

void MultimonitorController::disableMonitor()
{
    if(status != MultimonitorControllerStatus::IDLE && status != MultimonitorControllerStatus::CONNECTED) {
        return;
    }

    if(status == MultimonitorControllerStatus::IDLE) {
        status = MultimonitorControllerStatus::DISABLING;
        monitorDisableWatcher.setFuture(QtConcurrent::run(this, &MultimonitorController::runMonitorDisable));
    } else {
        status = MultimonitorControllerStatus::DISABLING;
        stopServer();
    }
}

MultimonitorControllerStatus MultimonitorController::getStatus()
{
    return status;
}


// ###################### Future Handlers ####################
void MultimonitorController::monitorSetupFinished()
{
    bool success = monitorSetupWatcher.result();
    if(success) {
        //startServer();
        status = MultimonitorControllerStatus::IDLE;
    } else {
        status = MultimonitorControllerStatus::UNSET;
    }
}

void MultimonitorController::monitorDisableFinished()
{
    status = MultimonitorControllerStatus::UNSET;
}


// #################### Process Handlers ######################
void MultimonitorController::serverProcessOutput()
{
    QString out = server.readAllStandardOutput();
    QString err = server.readAllStandardError();
    qDebug() << out << err;
}

void MultimonitorController::serverProcessStarted()
{
    status = MultimonitorControllerStatus::CONNECTED;
}

void MultimonitorController::serverProcessFinished(int, QProcess::ExitStatus)
{
    switch (status)
    {
    case MultimonitorControllerStatus::RESTARTING:
        runServerRestart();
        break;

    case MultimonitorControllerStatus::DISABLING:
        monitorDisableWatcher.setFuture(QtConcurrent::run(this, &MultimonitorController::runMonitorDisable));
        break;

    default:
        status = MultimonitorControllerStatus::IDLE;
        break;
    }
}


// ################ Actions #########################
bool MultimonitorController::runMonitorSetup()
{
    // get position
    QString absPos = QString("%1x%2").arg(clipX).arg(clipY);

    // enable and position display
    QProcess::execute("xrandr", {"--output", virtualName, "--auto", relativePos, mainName});
    QProcess::execute("xrandr", {"--output", virtualName, "--pos", absPos});
    QThread::sleep(1);

    // get monitors list
    QProcess checkSetup;
    checkSetup.start("xrandr", {"--listmonitors"});
    checkSetup.waitForFinished();

    // check display correctly positioned
    QString monitors = checkSetup.readAllStandardOutput();
    QString regEx = QString("%1 %2\\/.*x%3\\/.*\\+%4\\+%5  %1").arg(virtualName).arg(virtualWidth).arg(virtualHeight).arg(clipX).arg(clipY);
    QRegularExpression re(regEx);
    return re.match(monitors).hasMatch();
}

bool MultimonitorController::runMonitorDisable()
{
    // disable monitor
    QThread::sleep(2);
    QProcess::execute("xrandr", {"--output", virtualName, "--off"});
    return true;
}

void MultimonitorController::runServerRestart()
{
    // restart
    status = MultimonitorControllerStatus::CONNECTING;
    QTimer::singleShot(2000, this, [&]{
        startServer();
    });
}
