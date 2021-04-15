#include "plannerdashboard.h"
#include "schemehandler.h"

#include <QHotkey>
#include <QWebEngineProfile>

PlannerDashboard::PlannerDashboard(bool devTools)
{
    handler.install();
    registerShortcut();
    if(devTools)
        setupDevTools();
}

void PlannerDashboard::registerShortcut()
{
    QKeySequence k(Qt::Key_ScrollLock);
    auto hotkey = new QHotkey(k, true, qApp);
    QObject::connect(hotkey, &QHotkey::activated, qApp, [&](){
        view.toggleState();
    });
}

void PlannerDashboard::initializeApp()
{
    SchemeHandler::registerUrlScheme();
}

void PlannerDashboard::setupDevTools()
{
    view.page()->setDevToolsPage(devTools.page());
    devTools.show();
}
