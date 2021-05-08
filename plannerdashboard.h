#ifndef PLANNERDASHBOARD_H
#define PLANNERDASHBOARD_H

#include <QApplication>
#include <QWebEngineProfile>

#include "overlaywindow.h"
#include "droidcamcontroller.h"
#include "multimonitorcontroller.h"

class PlannerDashboard : public QObject
{
Q_OBJECT

public:
    PlannerDashboard();
    void setupDevTools();
    void showMessageBox(QString title, QString message);
    static void initializeApp();

    DroidcamController dc;
    MultimonitorController multimonitor;

private:
    OverlayWindow view;
    QWebEngineView devTools;
    SchemeHandler handler;
    QWebEngineProfile profile;

    void registerShortcut();
    void setupConfig();
};

#endif // PLANNERDASHBOARD_H
