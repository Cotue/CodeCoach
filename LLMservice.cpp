//
// Created by lavar on 30/11/2025.
//

#include "llmservice.h"
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUrlQuery>

LLMService::LLMService(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &LLMService::onNetworkReply);
}

void LLMService::solicitarPista(const QString &codigo, const QString &errores, const QString &salidaTests)
{
    QString baseUrl = "https://generativelanguage.googleapis.com/v1/models/gemini-2.5-flash:generateContent";
    QUrl url(baseUrl);

    QUrlQuery query;
    query.addQueryItem("key", API_KEY);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString promptFinal = QString(
        "Actúa como un profesor de programación C++. El estudiante tiene errores.\n"
        "CÓDIGO DEL ESTUDIANTE:\n%1\n\n"
        "ERRORES:\n%2\n\n"
        "TESTS:\n%3\n\n"
        "Da solo una pista breve. No soluciones el ejercicio."
    ).arg(codigo, errores, salidaTests);

    // Crear JSON
    QJsonObject textPart;
    textPart["text"] = promptFinal;

    QJsonArray parts;
    parts.append(textPart);

    QJsonObject content;
    content["role"] = "user";
    content["parts"] = parts;

    QJsonArray contents;
    contents.append(content);

    QJsonObject json;
    json["contents"] = contents;

    // Enviar el prompt
    m_manager->post(request, QJsonDocument(json).toJson());
}


void LLMService::onNetworkReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();

        // Parseo de la respuesta de GEMINI
        if (jsonObject.contains("candidates")) {
            QJsonArray candidates = jsonObject["candidates"].toArray();
            if (!candidates.isEmpty()) {
                QJsonObject content = candidates[0].toObject()["content"].toObject();
                QJsonArray parts = content["parts"].toArray();
                if (!parts.isEmpty()) {
                    QString textoRespuesta = parts[0].toObject()["text"].toString();
                    emit pistaRecibida(textoRespuesta);
                    reply->deleteLater();
                    return;
                }
            }
        }
        emit errorOcurrido("La respuesta de Gemini no tuvo el formato esperado.");
    } else {
        emit errorOcurrido("Error de red: " + reply->errorString());
    }
    reply->deleteLater();
    qDebug() << reply->error() << reply->errorString();
    qDebug() << reply->readAll();
}