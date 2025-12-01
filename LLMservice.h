//
// Created by lavar on 30/11/2025.
//

#ifndef MIINTERFAZQT_LLMSERVICE_H
#define MIINTERFAZQT_LLMSERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class LLMService : public QObject
    {
        Q_OBJECT
    public:
        explicit LLMService(QObject *parent = nullptr);
        void solicitarPista(const QString &codigo, const QString &errores, const QString &salidaTests);

        signals:
            void pistaRecibida(const QString &pista);
        void errorOcurrido(const QString &mensaje);

    private slots:
        void onNetworkReply(QNetworkReply *reply);

    private:
        QNetworkAccessManager *m_manager;
        const QString API_KEY = "AIzaSyDcCoLy3oyJOntUxVHXNiaTZYwRttBbIFI"; // OpenAI o Gemini
    };


#endif //MIINTERFAZQT_LLMSERVICE_H