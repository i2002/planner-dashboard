#ifndef PLANNERDASHBOARD_H
#define PLANNERDASHBOARD_H

#include <QApplication>
#include <QWebEngineProfile>
#include "overlaywindow.h"

class PlannerDashboard
{
public:
    PlannerDashboard();
    void setupDevTools();
    static void initializeApp();

private:
    OverlayWindow view;
    QWebEngineView devTools;
    SchemeHandler handler;
    QWebEngineProfile profile;

    void registerShortcut();
    void setupConfig();
};

#endif // PLANNERDASHBOARD_H
