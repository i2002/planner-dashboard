#ifndef OVERLAYWINDOW_H
#define OVERLAYWINDOW_H

#include "schemehandler.h"
#include "apppage.h"
#include <QWebEngineView>
#include <QEvent>
#include <QShortcut>

class OverlayWindow : public QWebEngineView
{
    Q_OBJECT
public:
    OverlayWindow(QWidget* parent = nullptr);
    bool event(QEvent* e);
    void toggleState();
    void setupPage();

private:
    void setupEvents();

    QShortcut closeShortcut;
    AppPage appPage;
};

#endif // OVERLAYWINDOW_H
