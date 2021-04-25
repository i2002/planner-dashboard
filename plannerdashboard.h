#ifndef PLANNERDASHBOARD_H
#define PLANNERDASHBOARD_H

#include <QApplication>
#include <QWebEngineProfile>

#include "overlaywindow.h"
#include "droidcamcontroller.h"

class PlannerDashboard : public QObject
{
Q_OBJECT

public:
    PlannerDashboard();
    void setupDevTools();
    static void initializeApp();

    DroidcamController dc;

private:
    OverlayWindow view;
    QWebEngineView devTools;
    SchemeHandler handler;
    QWebEngineProfile profile;

    void registerShortcut();
    void setupConfig();
};

#endif // PLANNERDASHBOARD_H
