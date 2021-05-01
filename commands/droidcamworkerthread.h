#ifndef DROIDCAMWORKERTHREAD_H
#define DROIDCAMWORKERTHREAD_H

#include <QThread>
#include <QProcess>

enum class DroidcamWorkerType {
    START_DC,
    STOP_DC
};

class DroidcamWorkerThread : public QThread
{
    Q_OBJECT
    void run() override;

signals:
    void resultReady(const QString &s);
    void message(const QString &s);
    void connectionError(const QString &reason);

public:
    DroidcamWorkerThread(DroidcamWorkerType type, QString ip, QString dcExec, QString dcPort, QString adbExec, QString adbPort, QString deviceId, QObject *parent = nullptr);
    void demo();

private:
    DroidcamWorkerType type;
    QString ip;
    QString dcExec;
    QString dcPort;
    QString dcPkgName;
    QString adbExec;
    QString adbPort;
    QString adbSocket;
    QString deviceId;
    QProcess* droidcam;

    QString getDevices();
    bool connectDevice();
    bool setupConnection();
    bool startApp();
    bool waitForUnlock();
    bool checkUnlocked();
};

#endif // DROIDCAMWORKERTHREAD_H
