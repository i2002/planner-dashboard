#include "apppage.h"

#include <QDesktopServices>

AppPage::AppPage(QWebEngineProfile* profile, QObject* parent) : QWebEnginePage(profile, parent)
{
}

AppPage::AppPage(QObject* parent) : QWebEnginePage(parent)
{
}


bool AppPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool)
{
    //TODO: zoommtg: url schemes
   if (type == QWebEnginePage::NavigationTypeLinkClicked)
   {
       QDesktopServices::openUrl(url);
       return false;
   }
   return true;
}
