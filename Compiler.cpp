//
// Created by evans on 6/11/2025.
//

#include "Compiler.h"
#include <QProcess>
#include <QTemporaryFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

Compiler::Compiler(QObject *parent) : QObject(parent) {}

void Compiler::processCode(const QString &code) {
    // Crear archivo .cpp temporal. Se utilizan 6 X porque QTemporaryFile toma esto sin necesidad de una ruta específica
    QTemporaryFile tempFile(QDir::temp().filePath("XXXXXX.cpp"));
    tempFile.setAutoRemove(false); // Control manual para borrar el archivo

    if (!tempFile.open()) {
        emit newOutput("Error: No se pudo crear el archivo temporal .cpp.");
        emit taskFinished();
        return;
    }
    QString cppFilePath = tempFile.fileName();
    qDebug() << "Archivo .cpp temporal: " << cppFilePath;

    QTextStream out(&tempFile);
    out << code;
    tempFile.close();

    // Ruta temporal del archivo
    // Creandolo igual en temp
    QString exeFilePath = cppFilePath;
    exeFilePath.replace(".cpp", ".exe");

    // Compilación
    emit newOutput("Iniciando compilación \n");
    QProcess compileProcess;
    QStringList args;
    args << cppFilePath << "-o" << exeFilePath;

    compileProcess.start("g++", args);

    // Espera indefinidamente hasta que el compilador termine
    compileProcess.waitForFinished(-1);

    QString compileErrors = compileProcess.readAllStandardError();
    if (!compileErrors.isEmpty()) {
        emit newOutput(compileErrors);
    }

    // En caso de que la compilación falle
    if (compileProcess.exitCode() != 0) {
        emit newOutput("\n --- Compilación fallida ---");
        QFile::remove(cppFilePath);
        emit taskFinished();
        return;
    }

    // En el caso exitoso
    emit newOutput("\n --- Compilación exitosa ---");
    emit newOutput("\nIniciando ejecución... \n\n");

    // Ejecución
    QProcess runProcess;
    runProcess.start(exeFilePath);

    // Esperando con un timeout de 10 segundos, esto para evitar que se congele el compilador
    if (!runProcess.waitForFinished(10000)) {
        runProcess.kill();
        emit newOutput("\n --- Error: El programa, se excedió del tiempo límite.");
    } else {
        // Si el programa termina a tiempo se lee su salida
        QString runOutput = runProcess.readAllStandardOutput();
        QString runErrors = runProcess.readAllStandardError();
        emit newOutput(runOutput);
        if (!runErrors.isEmpty()) {
            emit newOutput("\n --- Errores de ejecución ---" + runErrors);
        }
    }

    // Limpieza final
    QFile::remove(cppFilePath);
    QFile::remove(exeFilePath);

    emit newOutput("\n Fin del prcoceso");
    emit taskFinished();
}