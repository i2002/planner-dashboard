#include "webview.h"

Webview::Webview(QWidget *parent) :
    QWebEngineView(parent),
    close_shortcut{QKeySequence("Escape"), this}
{
    // connect handler to keypress
    QObject::connect(&close_shortcut, &QShortcut::activated, this, &Webview::closeState);
}

bool Webview::event(QEvent * e)
{
    if(e->type() == QEvent::WindowDeactivate && isVisible())
        hide();

    return QWebEngineView::event(e);
}

void Webview::toggleState()
{
    if(!isVisible())
        showFullScreen();
    else
        hide();
}

void Webview::closeState()
{
    if(isVisible())
        hide();
}
