#ifndef DROIDCAMCONTROLLER_H
#define DROIDCAMCONTROLLER_H

#include <QProcess>

#include <QFutureWatcher>

enum class AdbSetupStatus
{
    SUCCESFUL,
    DEVICE_OFFLINE,
    DEVICE_UNSET,
    DEVICE_UNAUTHORIZED,
    DEVICE_CONNECTION_FAILURE,
    APP_START_FAILURE,
    UNLOCK_TIMEOUT,
};

enum class DroidcamControllerStatus {
    IDLE,
    CONNECTING,
    CONNECTED
};

class DroidcamController : public QObject
{
    Q_OBJECT

public:
    DroidcamController(QObject* parent = nullptr);

    void start();
    void stop();
    DroidcamControllerStatus getStatus();

private:
    DroidcamControllerStatus status = DroidcamControllerStatus::IDLE;
    QProcess droidcam;
    QFutureWatcher<QPair<AdbSetupStatus, QString>> adbSetupWatcher;
    QFutureWatcher<AdbSetupStatus> adbCloseWatcher;

    // config data
    QString ip;
    QString dcExec;
    QString dcPort;
    QString dcPkgName;
    QString adbExec;
    QString adbPort;
    QString adbSocket;
    QString deviceId;

    // helpers
    void setupConfig();

    // handlers
    void adbSetupFinished();
    void adbCloseFinished();
    void dcProcessOutput();
    void dcProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    // adb comm
    QPair<AdbSetupStatus, QString> runAdbSetup();
    AdbSetupStatus runAdbClose();
    QString getDevices();
    QString connectDevice();
    bool setupConnection();
    bool startApp();
    bool waitForUnlock();
    bool checkUnlocked();
};

#endif // DROIDCAMCONTROLLER_H
