#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebEngineView>
#include <QEvent>
#include <QShortcut>

class Webview : public QWebEngineView
{
    Q_OBJECT
public:
    Webview(QWidget *parent = Q_NULLPTR);
    bool event(QEvent * e);
    void toggleState();
    void closeState();

private:
    QShortcut close_shortcut;
};

#endif // WEBVIEW_H
