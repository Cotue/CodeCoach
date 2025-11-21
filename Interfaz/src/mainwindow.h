#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "MongoService.h"

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
    void cargarMarkdownEnScrollArea(const QString& markdown);
    void crearEditorEnScrollArea2();
    QString rutaArchivoTemporal;
    MongoService mongo;
    bool primeraSeleccion = true;


private slots:
    void guardarCodigoTemporal();
    void cargarMarkdownPorDificultad(const QString& dificultad);

};

#endif // MAINWINDOW_H
