#include "apphandler.h"

#include <QApplication>
#include <QFile>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlScheme>

#define SCHEMENAME "app"

const QByteArray AppHandler::schemeName = QByteArrayLiteral(SCHEMENAME);
const QUrl AppHandler::aboutUrl = QUrl(QStringLiteral(SCHEMENAME ":index"));

AppHandler::AppHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler(parent)
{
}

void AppHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    static const QUrl webUiOrigin(QStringLiteral(SCHEMENAME ":"));
    static const QByteArray GET(QByteArrayLiteral("GET"));
    static const QByteArray POST(QByteArrayLiteral("POST"));

    QByteArray method = job->requestMethod();
    QUrl url = job->requestUrl();
    QUrl initiator = job->initiator();

    if (method == GET && url == aboutUrl) {
        QFile *file = new QFile(QStringLiteral(":/app/index.html"), job);
        file->open(QIODevice::ReadOnly);
        job->reply(QByteArrayLiteral("text/html"), file);
    } else if(method == GET && initiator == webUiOrigin) {
        QMimeType m = db.mimeTypeForUrl(url);
        QFile *file = new QFile(QStringLiteral(":/app/") + url.path());

        if(file->open(QIODevice::ReadOnly))
            job->reply(m.name().toUtf8(), file);
        else
            job->fail(QWebEngineUrlRequestJob::UrlNotFound);

    } else if (method == POST && url == aboutUrl && initiator == webUiOrigin) {
        job->fail(QWebEngineUrlRequestJob::RequestAborted);
        QApplication::exit();
    } else {
        job->fail(QWebEngineUrlRequestJob::UrlNotFound);
    }
}

// static
void AppHandler::registerUrlScheme()
{
    QWebEngineUrlScheme appScheme(schemeName);
    appScheme.setFlags(QWebEngineUrlScheme::SecureScheme |
                         QWebEngineUrlScheme::LocalScheme |
                         QWebEngineUrlScheme::LocalAccessAllowed);
    QWebEngineUrlScheme::registerScheme(appScheme);
}
