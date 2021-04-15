#include "schemehandler.h"

#include <QFile>
#include <QDir>

#include <QStandardPaths>
#include <QMimeType>

#include <QTextCodec>
#include <QDataStream>

#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlScheme>
#include <QWebEngineProfile>

#define SCHEMENAME "app"

const QByteArray SchemeHandler::schemeName = QByteArrayLiteral(SCHEMENAME);
const QUrl SchemeHandler::indexUrl = QUrl(QStringLiteral(SCHEMENAME ":index"));

SchemeHandler::SchemeHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler(parent)
{
}

void SchemeHandler::install()
{
    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler(SchemeHandler::schemeName, this);
}

void SchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    static const QUrl webUiOrigin(QStringLiteral(SCHEMENAME ":"));
    static const QByteArray GET(QByteArrayLiteral("GET"));
    static const QByteArray POST(QByteArrayLiteral("POST"));

    QByteArray method = job->requestMethod();
    QUrl url = job->requestUrl();
    QUrl initiator = job->initiator();

    if (method == GET && url == indexUrl) {
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        QFile *file = new QFile(QStringLiteral(":/app/index.html"), job);
        file->open(QIODevice::ReadOnly | QIODevice::Text);
        job->reply(QByteArrayLiteral("text/html"), file);

    } else if(method == GET && initiator == webUiOrigin && url.path() == "data") {
        QFile *data = new QFile(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/abc.json");
        QDir dir;
        if(!dir.exists(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))) {
            dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
        }

        if(!data->exists()) {
            data->open(QIODevice::ReadWrite);
            data->write("[[],[],[],[],[]]");
        }
        else {
            data->open(QIODevice::ReadOnly);
        }
        QDataStream s(QByteArrayLiteral("demo"));
        job->reply(QByteArrayLiteral("application/json"), data);
    } else if(method == GET && initiator == webUiOrigin) {
        QMimeType m = db.mimeTypeForUrl(url);
        QFile *file = new QFile(QStringLiteral(":/app/") + url.path());

        if(file->open(QIODevice::ReadOnly))
            job->reply(m.name().toUtf8(), file);
        else
            job->fail(QWebEngineUrlRequestJob::UrlNotFound);

    } else if (method == POST && url == indexUrl && initiator == webUiOrigin) {
        job->fail(QWebEngineUrlRequestJob::RequestAborted);
    } else {
        job->fail(QWebEngineUrlRequestJob::UrlNotFound);
    }
}


// static
void SchemeHandler::registerUrlScheme()
{
    QWebEngineUrlScheme appScheme(schemeName);
    appScheme.setFlags(QWebEngineUrlScheme::SecureScheme |
                         QWebEngineUrlScheme::LocalScheme |
                         QWebEngineUrlScheme::LocalAccessAllowed |
                         QWebEngineUrlScheme::CorsEnabled);
    appScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    QWebEngineUrlScheme::registerScheme(appScheme);
}
