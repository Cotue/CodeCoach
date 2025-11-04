#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTextStream>
#include <QTextBrowser>
#include <QDebug>
#include <QCoreApplication>
#include "CppHighlighter.h"
#include "CodeEditor.h"
#include <QPlainTextEdit>
#include <QDir>
#include <QFile>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString rutaArchivo = QDir(QCoreApplication::applicationDirPath())
                      .filePath("../markdowns/prueba.md");
    cargarMarkdownEnScrollArea(rutaArchivo);
    crearEditorEnScrollArea2();
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::guardarCodigoTemporal);
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
    // 🧱 Crear el editor de código
    CodeEditor* editor = new CodeEditor(this);
    new CppHighlighter(editor->document());
    editor->setPlaceholderText("// Escribe tu código aquí...\n");

    // 📦 Obtener el widget contenido dentro del scrollArea
    QWidget* contenido = ui->scrollArea_2->widget();
    if (!contenido) {
        qWarning("scrollArea_2 no tiene contenido interno asignado desde Qt Designer");
        return;
    }

    // 📐 Obtener el layout vertical ya existente (verticalLayout_2)
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(contenido->layout());
    if (!layout) {
        layout = new QVBoxLayout(contenido);
        contenido->setLayout(layout);
    }

    // 🔍 El layout ya tiene el pushButton del .ui, así que solo agregamos el editor antes de él
    layout->insertWidget(0, editor);  // Insertar el editor arriba del botón

    // (opcional) Margen visual
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(6);
}


void MainWindow::guardarCodigoTemporal()
{
    // Buscar el editor (CodeEditor o QPlainTextEdit) dentro de scrollArea_2
    QWidget* contenido = ui->scrollArea_2->widget();
    if (!contenido) {
        QMessageBox::warning(this, "Error", "No hay contenido en scrollArea_2.");
        return;
    }

    QPlainTextEdit* editor = contenido->findChild<QPlainTextEdit*>();
    if (!editor) {
        QMessageBox::warning(this, "Error", "No se encontró el editor de código en scrollArea_2.");
        return;
    }

    QString codigo = editor->toPlainText();
    if (codigo.isEmpty()) {
        QMessageBox::information(this, "Aviso", "El código está vacío, nada que guardar.");
        return;
    }

    // 🔸 Crear carpeta temp junto al ejecutable
    QString rutaTemp = QCoreApplication::applicationDirPath() + "/temp";
    QDir dir(rutaTemp);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // 🔸 Guardar archivo temporal
    rutaArchivoTemporal = dir.filePath("temp_code.cpp");
    QFile archivo(rutaArchivoTemporal);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo crear el archivo temporal.");
        return;
    }

    QTextStream salida(&archivo);
    salida << codigo;
    archivo.close();

    QMessageBox::information(this, "Guardado exitoso",
        "Código guardado en:\n" + rutaArchivoTemporal);
}