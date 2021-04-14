#include "mainwindow.h"

#include "apphandler.h"
#include "appview.h"

#include <QApplication>
#include <QHotkey>

int main(int argc, char *argv[])
{
    AppHandler::registerUrlScheme();
    QApplication a(argc, argv);

    AppView webView;
    QKeySequence k(Qt::Key_ScrollLock);
    auto hotkey = new QHotkey(k, true, &a);
    QObject::connect(hotkey, &QHotkey::activated, qApp, [&](){
        webView.view.toggleState();
    });
    return a.exec();
}
