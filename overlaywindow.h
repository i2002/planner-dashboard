#ifndef OVERLAYWINDOW_H
#define OVERLAYWINDOW_H

#include "schemehandler.h"
#include <QWebEngineView>
#include <QEvent>
#include <QShortcut>

class OverlayWindow : public QWebEngineView
{
    Q_OBJECT
public:
    OverlayWindow(QWidget* parent = nullptr);
    //~OverlayWindow();
    bool event(QEvent* e);
    void toggleState();
    void setupPage();

private:
    void setupEvents();

    QShortcut closeShortcut;
};

#endif // OVERLAYWINDOW_H
