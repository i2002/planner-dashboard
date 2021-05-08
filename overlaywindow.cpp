#include "overlaywindow.h"
#include "schemehandler.h"

OverlayWindow::OverlayWindow(QWidget* parent) :
    QWebEngineView(parent),
    closeShortcut{Qt::Key_Escape, this},
    appPage{this}
{
    setupPage();
    setupEvents();
}

bool OverlayWindow::event(QEvent* e)
{
    if(e->type() == QEvent::WindowDeactivate && isVisible() && !keep)
        hide();

    return QWebEngineView::event(e);
}

void OverlayWindow::toggleState()
{
    if(!isVisible()) {
        showMaximized();
        activateWindow();
    } else
        hide();
}

void OverlayWindow::setupPage()
{
    appPage.load(SchemeHandler::indexUrl);
    appPage.setBackgroundColor(Qt::transparent);

    setPage(&appPage);
    setContextMenuPolicy(Qt::NoContextMenu);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    resize(1920, 900);
}

void OverlayWindow::setKeep(bool val)
{
    keep = val;
}

void OverlayWindow::setupEvents()
{
    QObject::connect(&closeShortcut, &QShortcut::activated, this, [&](){
        if(isVisible())
            hide();
    });
}
