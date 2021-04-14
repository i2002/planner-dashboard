#include "appview.h"

AppView::AppView() : page {&this->profile}
{
    this->installUrlHandler();
    this->createPage();
    this->setupView();
}

void AppView::installUrlHandler()
{
    profile.installUrlSchemeHandler(AppHandler::schemeName, &this->handler);
}

void AppView::createPage()
{
    page.load(AppHandler::aboutUrl);
    page.setBackgroundColor(Qt::transparent);
    page.setDevToolsPage(&page);
}

void AppView::setupView()
{
    page.setDevToolsPage(dev.page());
    dev.show();
    view.setAttribute(Qt::WA_TranslucentBackground);
    view.setPage(&page);
    view.setContextMenuPolicy(Qt::NoContextMenu);
}
