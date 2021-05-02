#include "schemehandler.h"

#include "plannerdashboard.h"

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
const QUrl SchemeHandler::dataUrl = QUrl(QStringLiteral(SCHEMENAME ":data"));

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
        handleInput(job, url);
    }
}

void SchemeHandler::serveFiles(QWebEngineUrlRequestJob *job, QUrl url)
{
    QMimeType m = db.mimeTypeForUrl(url);
    QFile *file = new QFile(QStringLiteral(":/app/") + url.path());

    if(file->open(QIODevice::ReadOnly)) {
        job->reply(m.name().toUtf8(), file);
        connect(job, &QWebEngineUrlRequestJob::destroyed, file, &QObject::deleteLater);
    } else
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
            connect(job, &QWebEngineUrlRequestJob::destroyed, data, &QObject::deleteLater);
            return;
        }

        else if(action == "droidcam-status") {
            QString status = "";
            switch (((PlannerDashboard *)parent())->dc.getStatus())
            {
            case DroidcamControllerStatus::IDLE:
                status = "idle";
                break;
            case DroidcamControllerStatus::CONNECTING:
                status = "connecting";
                break;
            case DroidcamControllerStatus::CONNECTED:
                status = "connected";
                break;
            }

            job->reply(QByteArrayLiteral("application/json"), replyString(job, status));
        }

        else if(action == "multimonitor-status") {
            QString status = "";
            switch (((PlannerDashboard *)parent())->multimonitor.getStatus())
            {
            case MultimonitorControllerStatus::UNSET:
                status = "unset";
                break;
            case MultimonitorControllerStatus::IDLE:
                status = "idle";
                break;
            case MultimonitorControllerStatus::CONNECTING:
            case MultimonitorControllerStatus::RESTARTING:
            case MultimonitorControllerStatus::DISABLING:
                status = "connecting";
                break;

            case MultimonitorControllerStatus::CONNECTED:
                status = "connected";
                break;
            }

            job->reply(QByteArrayLiteral("application/json"), replyString(job, status));
        }
    }

    job->fail(QWebEngineUrlRequestJob::RequestAborted);
}

void SchemeHandler::handleInput(QWebEngineUrlRequestJob *job, QUrl url)
{
    QRegularExpression re("^action=([\\w-]+)");
    QRegularExpressionMatch match = re.match(url.query());
    if (match.hasMatch()) {
        QString action = match.captured(1);
        if(action == "droidcam-start") {
            ((PlannerDashboard *)parent())->dc.start();
            job->reply("application/json", replyString(job, "done"));
            return;
        }
        else if(action == "droidcam-stop") {
            ((PlannerDashboard *)parent())->dc.stop();
            job->reply("application/json", replyString(job, "done"));
            return;
        }
        else if(action == "multimonitor-setup") {
            // get monitor position
            MultiMonitorPosition monitorPos = MultiMonitorPosition::LEFT;
            QRegularExpression rePos("position=([\\w-]+)");
            QRegularExpressionMatch matchPos = rePos.match(url.query());
            if(matchPos.hasMatch()) {
                QString position = matchPos.captured(1);
                monitorPos = position == "left" ? MultiMonitorPosition::LEFT : MultiMonitorPosition::RIGHT;
            }

            ((PlannerDashboard *)parent())->multimonitor.setupMonitor(monitorPos);
            job->reply("application/json", replyString(job, "done"));
            return;
        }
        else if(action == "multimonitor-reset") {
            ((PlannerDashboard *)parent())->multimonitor.restartServer();
            job->reply("application/json", replyString(job, "done"));
            return;
        }
        else if(action == "multimonitor-disable") {
            ((PlannerDashboard *)parent())->multimonitor.disableMonitor();
            job->reply("application/json", replyString(job, "done"));
            return;
        }


    }

    job->fail(QWebEngineUrlRequestJob::RequestAborted);
}

QBuffer* SchemeHandler::replyString(QWebEngineUrlRequestJob* job, QString s)
{
    // create buffer for QString
    QBuffer *buffer = new QBuffer;
    buffer->open(QIODevice::WriteOnly);
    buffer->write(s.toUtf8());

    // delete buffer after job destroy
    connect(job, &QWebEngineUrlRequestJob::destroyed, buffer, &QObject::deleteLater);
    buffer->close();

    return buffer;
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
