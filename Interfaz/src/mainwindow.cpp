#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFile>
#include <QTextStream>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString rutaArchivo = QDir(QCoreApplication::applicationDirPath())
                      .filePath("../markdowns/prueba.md");
    cargarMarkdownEnScrollArea(rutaArchivo);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::cargarMarkdownEnScrollArea(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "No se pudo abrir el archivo:" << filePath;
        return;
    }

    QTextStream in(&file);
    QString markdown = in.readAll();
    file.close();

    // Crear QTextBrowser
    QTextBrowser* browser = new QTextBrowser(this);
    browser->setMarkdown(markdown); // ✅ Solo disponible en Qt 6.4+
    browser->setOpenExternalLinks(true);
    browser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Obtener el contenido interno de scrollArea1
    QWidget* contenido = ui->scrollArea1->widget();
    if (!contenido) {
        contenido = new QWidget();
        ui->scrollArea1->setWidget(contenido);
    }

    // Asegurar layout vertical
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(contenido->layout());
    if (!layout) {
        layout = new QVBoxLayout(contenido);
        contenido->setLayout(layout);
    }

    // Limpiar contenido previo
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    layout->addWidget(browser);
}