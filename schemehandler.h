#ifndef SCHEMEHANDLER_H
#define SCHEMEHANDLER_H

#include <QWebEngineUrlSchemeHandler>
#include <QMimeDatabase>

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

private:
    QMimeDatabase db;
};

#endif // SCHEMEHANDLER_H
