#ifndef APPHANDLER_H
#define APPHANDLER_H

#include <QWebEngineUrlSchemeHandler>

#include <QMimeType>
#include <QMimeDatabase>

class AppHandler : public QWebEngineUrlSchemeHandler
{
        Q_OBJECT
public:
    explicit AppHandler(QObject *parent = nullptr);

    void requestStarted(QWebEngineUrlRequestJob *job) override;

    static void registerUrlScheme();
    const static QByteArray schemeName;
    const static QUrl aboutUrl;

private:
    QMimeDatabase db;
};

#endif // APPHANDLER_H
