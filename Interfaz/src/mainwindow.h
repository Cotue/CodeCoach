#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Compiler.h"
#include <QThread>
#include <LLMservice.h>
#include <CodeEditor.h>

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
    void crearEditorEnScrollArea2();
    void on_btnPedirPista();
    QString rutaArchivoTemporal;
    CodeEditor *m_CodeEditor = nullptr;

    LLMService *m_llmService;
    QString m_ultimoCodigo;
    QString m_ultimaSalida;
};

#endif // MAINWINDOW_H
