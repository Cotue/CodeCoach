#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Compiler.h"
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QThread compilerThread; // Hilo del compilador.
    Compiler *compiler; // Puntero al compilador.
    void cargarMarkdownEnScrollArea(const QString& filePath);
    void crearEditorEnScrollArea2();
    QString rutaArchivoTemporal;


private slots:
    void enviarCodigo();
    // Slots dedicados al compilador, reaccionando a las señales que transmite
    void handleNewOutput(const QString &output);
    void handleTaskFinished();

    // Señal para comunicarse con el hilo de forma segura
    signals:
        void startProcessing(const QString &code);
};

#endif // MAINWINDOW_H
