#include "schemehandler.h"

#include <QFile>
#include <QDir>

#include <QStandardPaths>
#include <QMimeType>

//#include <QTextCodec>
#include <QDataStream>

#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlScheme>
#include <QWebEngineProfile>

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#define SCHEMENAME "app"

const QByteArray SchemeHandler::schemeName = QByteArrayLiteral(SCHEMENAME);
const QUrl SchemeHandler::indexUrl = QUrl(QStringLiteral(SCHEMENAME ":index.html"));
const QUrl dataUrl = QUrl(QStringLiteral(SCHEMENAME ":data"));

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

    if((method != GET && method != POST) || (initiator != webUiOrigin && url != indexUrl)) {
        job->fail(QWebEngineUrlRequestJob::RequestDenied);
        return;
    }

    if(method == GET) {
        if(url.path() == dataUrl.path()) {
            sendData(job, url);
        }
        else {
            serveFiles(job, url);
        }
    }

    if(method == POST) {

    }
}

void SchemeHandler::serveFiles(QWebEngineUrlRequestJob *job, QUrl url)
{
    QMimeType m = db.mimeTypeForUrl(url);
    QFile *file = new QFile(QStringLiteral(":/app/") + url.path());

    if(file->open(QIODevice::ReadOnly))
        job->reply(m.name().toUtf8(), file);
    else
        job->fail(QWebEngineUrlRequestJob::UrlNotFound);

}

void SchemeHandler::sendData(QWebEngineUrlRequestJob *job, QUrl url)
{
    QRegularExpression re("^action=([\\w-]+)");
    QRegularExpressionMatch match = re.match(url.query());
    if (match.hasMatch()) {
        QString action = match.captured(1);
        if(action == "school-schedule" || action == "school-subjects") {
            QFile *data = new QFile(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/" + action + ".json");
            data->open(QIODevice::ReadOnly);
            job->reply(QByteArrayLiteral("application/json"), data);
            return;
        }
    }

    job->fail(QWebEngineUrlRequestJob::RequestAborted);
}

void SchemeHandler::handleInput(QWebEngineUrlRequestJob *job, QUrl url)
{

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
