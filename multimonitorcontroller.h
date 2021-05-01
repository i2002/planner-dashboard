#ifndef MULTIMONITORCONTROLLER_H
#define MULTIMONITORCONTROLLER_H

#include <QObject>
#include <QProcess>
#include <QFutureWatcher>

enum class MultimonitorControllerStatus {
    UNSET,
    IDLE,
    CONNECTING,
    CONNECTED,
    RESTARTING,
    DISABLING
};

enum class MultiMonitorPosition {
    RIGHT,
    LEFT
};

class MultimonitorController : public QObject
{
    Q_OBJECT
public:
    explicit MultimonitorController(QObject *parent = nullptr);
    void setupMonitor(MultiMonitorPosition pos);
    void restartServer();
    void disableMonitor();
    MultimonitorControllerStatus getStatus();

private:
    MultimonitorControllerStatus status = MultimonitorControllerStatus::UNSET;
    QProcess server;
    QFutureWatcher<bool> monitorSetupWatcher;
    QFutureWatcher<bool> monitorDisableWatcher;

    // config data
    int mainWidth;
    int mainHeight;
    QString mainName;
    int virtualWidth;
    int virtualHeight;
    QString virtualName;
    QString serverPassword;

    // position data
    MultiMonitorPosition pos;
    int clipX;
    int clipY;
    QString relativePos;

    // helpers
    void setupConfig();
    void setPosition(MultiMonitorPosition p);

    // handlers
    void monitorSetupFinished();
    void monitorDisableFinished();
    void serverProcessOutput();
    void serverProcessStarted();
    void serverProcessFinished(int exitCode, QProcess::ExitStatus existStatus);

    // actions
    void startServer();
    void stopServer();
    bool runMonitorSetup();
    bool runMonitorDisable();
    void runServerRestart();

signals:

};

#endif // MULTIMONITORCONTROLLER_H
