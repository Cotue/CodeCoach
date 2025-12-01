#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Compiler.h"
#include <QThread>
#include <LLMservice.h>
#include <CodeEditor.h>
#include <QThread>

#include "MongoService.h"
#include "Compiler.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void startProcessing(const QString &code);

private slots:
    void enviarCodigo();
    void handleNewOutput(const QString &output);
    void handleTaskFinished();

private:
    Ui::MainWindow *ui;
    QThread compilerThread;
    Compiler *compiler;
    void cargarMarkdownEnScrollArea(const QString& filePath);
    QThread compilerThread; // Hilo del compilador.
    Compiler *compiler; // Puntero al compilador.
    void cargarMarkdownEnScrollArea(const QString& markdown);
    void crearEditorEnScrollArea2();
    void on_btnPedirPista();
    QString rutaArchivoTemporal;
    CodeEditor *m_CodeEditor = nullptr;

    LLMService *m_llmService;
    QString m_ultimoCodigo;
    QString m_ultimaSalida;
    MongoService mongo;
    bool primeraSeleccion = true;


private slots:
    void enviarCodigo();
    void cargarMarkdownPorDificultad(const QString& dificultad);
    // Slots dedicados al compilador, reaccionando a las señales que transmite
    void handleNewOutput(const QString &output);
    void handleTaskFinished();

    // Señal para comunicarse con el hilo de forma segura
    signals:
        void startProcessing(const QString &code);

};

#endif // MAINWINDOW_H
