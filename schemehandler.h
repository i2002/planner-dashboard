#ifndef SCHEMEHANDLER_H
#define SCHEMEHANDLER_H

#include <QWebEngineUrlSchemeHandler>
#include <QMimeDatabase>
#include <QBuffer>

class SchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT
public:
    explicit SchemeHandler(QObject *parent = nullptr);
    void install();
    void requestStarted(QWebEngineUrlRequestJob *job) override;

    static void registerUrlScheme();
    const static QByteArray schemeName;
    const static QUrl indexUrl;
    const static QUrl dataUrl;

private:
    QMimeDatabase db;

    void serveFiles(QWebEngineUrlRequestJob *job, QUrl url);
    void sendData(QWebEngineUrlRequestJob *job, QUrl url);
    void handleInput(QWebEngineUrlRequestJob *job, QUrl url);
    QBuffer* replyString(QWebEngineUrlRequestJob* job, QString s);
};

#endif // SCHEMEHANDLER_H
