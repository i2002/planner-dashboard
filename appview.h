#ifndef APPVIEW_H
#define APPVIEW_H

#include "apphandler.h"
#include "webview.h"

#include <QWebEngineProfile>
#include <QWebEnginePage>

class AppView
{
public:
    AppView();

    AppHandler handler;
    QWebEngineProfile profile;
    QWebEnginePage page;
    Webview view;
    QWebEngineView dev;

private:
    void installUrlHandler();
    void createPage();
    void setupView();
};

#endif // APPVIEW_H
