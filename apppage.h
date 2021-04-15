#ifndef APPPAGE_H
#define APPPAGE_H

#include <QWebEnginePage>

class AppPage : public QWebEnginePage
{
    Q_OBJECT
public:
    AppPage(QWebEngineProfile* profile, QObject* parent = nullptr);
    AppPage(QObject* parent = nullptr);
    bool acceptNavigationRequest(const QUrl & url, QWebEnginePage::NavigationType type, bool isMainFrame);
};

#endif // APPPAGE_H
