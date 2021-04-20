#include "overlaywindow.h"
#include "schemehandler.h"
#include "apppage.h"

OverlayWindow::OverlayWindow(QWidget* parent) :
    QWebEngineView(parent),
    closeShortcut{Qt::Key_Escape, this}
{
    setupPage();
    setupEvents();
}

//OverlayWindow::~OverlayWindow()
//{
//    //delete page();
//    QWebEngineView::~QWebEngineView();
//}

bool OverlayWindow::event(QEvent* e)
{
    if(e->type() == QEvent::WindowDeactivate && isVisible())
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
    AppPage* page = new AppPage();
    page->load(SchemeHandler::indexUrl);
    page->setBackgroundColor(Qt::transparent);

    setPage(page);
    setContextMenuPolicy(Qt::NoContextMenu);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    resize(1920, 900);
}

void OverlayWindow::setupEvents()
{
    QObject::connect(&closeShortcut, &QShortcut::activated, this, [&](){
        if(isVisible())
            hide();
    });
}
