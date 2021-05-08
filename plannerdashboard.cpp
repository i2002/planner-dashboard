#include "plannerdashboard.h"
#include "schemehandler.h"

#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QHotkey>
#include <QMessageBox>
#include <QThread>

//#include "commands/droidcamworkerthread.h"
#include <QMessageBox>

PlannerDashboard::PlannerDashboard() :
    dc{this},
    handler{this}
{
    handler.install();
    registerShortcut();
    setupConfig();
}

void PlannerDashboard::registerShortcut()
{
#ifdef QT_DEBUG
    QKeySequence k(Qt::SHIFT + Qt::Key_ScrollLock);
#endif

#ifndef QT_DEBUG
    QKeySequence k(Qt::Key_ScrollLock);
#endif

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

void PlannerDashboard::showMessageBox(QString title, QString message)
{
    // a window must be shown, otherwise the dialog is the last window closed and it closes application
    view.setKeep(true);
    view.showMaximized();
    QMessageBox::information(&view, title, message);
    view.setKeep(false);
}

void PlannerDashboard::setupConfig()
{
    // create app config folder if it doesn't exist
    QDir configDir;
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/";
    if(!configDir.exists(configPath)) {
        configDir.mkpath(configPath);
    }

    // create default config files if they don't exist
    QDir defaultConfigs(":/config");
    for(QString &file : defaultConfigs.entryList(QDir::Files)) {
        if(!QFile::exists(configPath + file)) {
            QFile::copy(":/config/" + file, configPath + file);
            QFile::setPermissions(configPath + file, QFileDevice::ReadOwner|QFileDevice::WriteOwner);
        }
    }

    // set organization name for QSettings
//    QCoreApplication::setOrganizationName("i2002");
//    QCoreApplication::setApplicationName("Planner Dashboard");
}
