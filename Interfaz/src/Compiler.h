//
// Created by evans on 6/11/2025.
//

#ifndef MIINTERFAZQT_COMPILADOR_H
#define MIINTERFAZQT_COMPILADOR_H

#include <QObject>
#include <QString>
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


class Compiler: public QObject {
    Q_OBJECT // Esta marca es obligatoria para cuando se utilizan signals o slots

    public:
        explicit Compiler(QObject *parent = nullptr);

    public slots:
        // Slot que será llamado por la GUI para que comience el compilador
        void processCode(const QString &code);
    signals:
        // Señal utilizada para enviar la salida
        void newOutput(const QString &output);
        // Señal que avisa cuando el proceso finaliza
        void taskFinished();
    private:
        QString construirCodigoFinal(const QString &code, const QJsonObject &configEjercicio);
        static QJsonObject cargarEjercicioDesdeArchivo(const QString &rutaArchivo);
};

#endif //MIINTERFAZQT_COMPILADOR_H