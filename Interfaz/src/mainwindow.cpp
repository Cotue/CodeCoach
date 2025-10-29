#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QFont>
#include <QScrollArea>
#include <QTextStream>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include "CppHighlighter.h"
#include "CodeEditor.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString rutaArchivo = QDir(QCoreApplication::applicationDirPath())
                      .filePath("../markdowns/prueba.md");
    cargarMarkdownEnScrollArea(rutaArchivo);
    crearEditorEnScrollArea2();
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
void MainWindow::crearEditorEnScrollArea2()
{
    // 🧱 Editor con números de línea, línea actual y fondo oscuro
    CodeEditor* editor = new CodeEditor(this);

    // ✨ Resaltado de sintaxis C++
    new CppHighlighter(editor->document());

    editor->setPlaceholderText("// Escribe tu código aquí...\n");

    // 📦 Obtener el widget contenido dentro del scrollArea
    QWidget* contenido = ui->scrollArea_2->widget();
    if (!contenido) {
        qWarning("scrollArea_2 no tiene contenido interno asignado desde Qt Designer");
        return;
    }

    // 📐 Obtener o crear el layout dentro del contenido
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(contenido->layout());
    if (!layout) {
        layout = new QVBoxLayout(contenido);
        contenido->setLayout(layout);
    }

    // 🧹 Limpiar cualquier widget previo dentro del layout
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    // ➕ Agregar el editor al layout
    layout->addWidget(editor);
}