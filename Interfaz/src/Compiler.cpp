//
// Created by evans on 6/11/2025.
//

#include "Compiler.h"
#include <QProcess>
#include <QTemporaryFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>

Compiler::Compiler(QObject *parent) : QObject(parent) {}

QJsonObject Compiler::cargarEjercicioDesdeArchivo(const QString &rutaArchivo) {
    QFile archivo(rutaArchivo);

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: No se pudo abrir el archivo JSON en:" << rutaArchivo;
        return {}; // Retorna objeto vacío
    }

    QString contenido = archivo.readAll();
    archivo.close();

    QJsonDocument doc = QJsonDocument::fromJson(contenido.toUtf8());

    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Error: El archivo no es un JSON válido o no es un objeto.";
        return QJsonObject();
    }

    return doc.object();
}

void Compiler::processCode(const QString &codigoUsuario) {

    // 1. [NUEVO] Simulación de carga del JSON (Esto luego vendrá de un archivo)
    // Supongamos que el ejercicio es: "Crear una función 'suma' que sume dos enteros"
    QJsonObject configEjercicio;
    configEjercicio["methodName"] = "sumar";
    configEjercicio["returnType"] = "int";

    QJsonArray casosPrueba;

    QJsonObject caso1; caso1["args"] = "3, 5"; caso1["expected"] = "8";
    casosPrueba.append(caso1);

    QJsonObject caso2; caso2["args"] = "-2, 4"; caso2["expected"] = "2";
    casosPrueba.append(caso2);

    QJsonObject caso3; caso3["args"] = "0, 0"; caso3["expected"] = "0";
    casosPrueba.append(caso3);

    configEjercicio["testCases"] = casosPrueba;

    // Se geenera el código completo mezclando el del usuario y el main
    QString codigoCompleto = construirCodigoFinal(codigoUsuario, configEjercicio);

    // Usar 6 X para la plantilla, asegurando extensión .cpp
    QTemporaryFile tempFile(QDir::temp().filePath("programa_XXXXXX.cpp"));
    tempFile.setAutoRemove(false);

    if (!tempFile.open()) {
        emit newOutput("Error: No se pudo crear el archivo temporal .cpp.");
        emit taskFinished();
        return;
    }
    QString cppFilePath = tempFile.fileName();
    qDebug() << "Archivo .cpp temporal: " << cppFilePath;

    QTextStream out(&tempFile);
    out << codigoCompleto;

    tempFile.close();

    // Ruta del ejecutable
    QString exeFilePath = cppFilePath;
    exeFilePath.replace(".cpp", ".exe");

    // Compilación
    emit newOutput("Iniciando compilación del caso de prueba... \n");
    QProcess compileProcess;
    QStringList args;
    args << cppFilePath << "-o" << exeFilePath;

    compileProcess.start("g++", args);
    compileProcess.waitForFinished(-1);

    QString compileErrors = compileProcess.readAllStandardError();
    if (!compileErrors.isEmpty()) {
        emit newOutput(compileErrors);
    }

    if (compileProcess.exitCode() != 0) {
        emit newOutput("--- Compilación fallida ---");
        // Si falla, borramos archivos y terminamos
        QFile::remove(cppFilePath);
        emit taskFinished();
        return;
    }

    emit newOutput("--- Compilación exitosa ---");
    emit newOutput("\nEjecutando casos de prueba... \n");

    // Ejecución
    QProcess runProcess;
    runProcess.start(exeFilePath);

    if (!runProcess.waitForFinished(10000)) {
        runProcess.kill();
        emit newOutput("--- Error: Time Limit Exceeded (10s).");
    } else {
        QString runOutput = runProcess.readAllStandardOutput();
        QString runErrors = runProcess.readAllStandardError();

        // Aquí verás "Caso 1: PASS", etc.
        emit newOutput(runOutput);

        if (!runErrors.isEmpty()) {
            emit newOutput(" --- Runtime Error ---\n" + runErrors);
        }
    }

    // Limpieza final
    QFile::remove(cppFilePath);
    QFile::remove(exeFilePath);

    emit newOutput("\nFin del proceso");
    emit taskFinished();
}

// --- NUEVA FUNCIÓN PRIVADA ---
QString Compiler::construirCodigoFinal(const QString &codigoUsuario, const QJsonObject &config) {
    QString final;

    // Headers necesarios para que el test funcione
    final += "#include <iostream>\n";
    final += "#include <vector>\n";
    final += "#include <string>\n";
    final += "#include <algorithm>\n\n";

    // Inyección del código del usuario
    final += "// --- CODIGO USUARIO ---\n";
    final += codigoUsuario + "\n";
    final += "// ----------------------\n\n";

    // Generación del Main
    final += "int main() {\n";
    final += "    Solution sol;\n"; // Instanciamos la clase del usuario
    final += "    bool allPassed = true;\n\n";

    QString metodo = config["methodName"].toString();
    QString tipo = config["returnType"].toString();
    QJsonArray casos = config["testCases"].toArray();

    int i = 1;
    for(const QJsonValue &val : casos) {
        QJsonObject caso = val.toObject();
        QString args = caso["args"].toString();
        QString expected = caso["expected"].toString();

        // Bloque de prueba para C++
        final += QString("    // Test %1: Entrada: (%2), Esperado: %3\n").arg(i).arg(args).arg(expected);
        final += QString("    %1 res%2 = sol.%3(%4);\n").arg(tipo).arg(i).arg(metodo).arg(args);

        // Comparación y resultados obtenidos
        // 1. Evaluar resultado
        final += QString("    bool correcto%1 = (res%1 == %2);\n").arg(i).arg(expected);
        final += QString("    if (!correcto%1) allPassed = false;\n\n").arg(i);

        // 2. Imprimir encabezado del caso
        final += QString("    std::cout << \"Caso %1: \" << (correcto%1 ? \"[CORRECTO]\" : \"[FALLO]\") << std::endl;\n").arg(i);

        // Imprimir detalles sobre la evaluación: entrada, salida y lo esperado
        final += QString("    std::cout << \"   Entrada: (%1)\" << std::endl;\n").arg(args);
        final += QString("    std::cout << \"   Esperado: %1\" << std::endl;\n").arg(expected);
        final += QString("    std::cout << \"   Obtenido: \" << res%1 << std::endl << std::endl;\n").arg(i); // Doble endl para separar casos

        i++;
    }

    final += "    return allPassed ? 0 : 1;\n";
    final += "}\n";

    return final;
}