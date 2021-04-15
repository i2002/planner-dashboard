#ifndef PLANNERDASHBOARD_H
#define PLANNERDASHBOARD_H

#include <QApplication>
#include <QWebEngineProfile>
#include "overlaywindow.h"

class PlannerDashboard
{
public:
    PlannerDashboard(bool devTools = false);
    static void initializeApp();

private:
    OverlayWindow view;
    QWebEngineView devTools;
    SchemeHandler handler;
    QWebEngineProfile profile;

    void registerShortcut();
    void setupDevTools();
};

#endif // PLANNERDASHBOARD_H
