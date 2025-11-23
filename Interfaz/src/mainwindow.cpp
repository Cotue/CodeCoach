#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTextStream>
#include <QDebug>
#include <QCoreApplication>
#include "CppHighlighter.h"
#include "CodeEditor.h"
#include <QPlainTextEdit>
#include <QFile>
#include <QTextBrowser>
#include <QDir>
#include <QMessageBox>
#include <QThread>
#include "Compiler.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    crearEditorEnScrollArea2();
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::enviarCodigo);
    connect(ui->comboBox, &QComboBox::textActivated,
        this, &MainWindow::cargarMarkdownPorDificultad);
    qDebug() << "[DEBUG] Constructor iniciado";
    // Configuración del hilo y el compilador

    // Compilador
    compiler = new Compiler;

    // Mover el comopilador al hilo
    compiler->moveToThread(&compilerThread);
    // 3. Conectamos las señales

    // Conexión para iniciar el trabajo (GUI -> Worker)
    connect(this, &MainWindow::startProcessing, compiler, &Compiler::processCode);

    // Conexiones para recibir resultados (Worker -> GUI)
    connect(compiler, &Compiler::newOutput, this, &MainWindow::handleNewOutput);
    connect(compiler, &Compiler::taskFinished, this, &MainWindow::handleTaskFinished);

    // Conexión para limpiar el worker cuando el hilo termine
    connect(&compilerThread, &QThread::finished, compiler, &QObject::deleteLater);

    // 4. Iniciamos el hilo. Ahora está "dormido" esperando señales.
    compilerThread.start();

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::cargarMarkdownEnScrollArea(const QString& markdown)
{
    QWidget* contenido = ui->scrollArea1->widget();
    if (!contenido) {
        contenido = new QWidget();
        ui->scrollArea1->setWidget(contenido);
    }

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(contenido->layout());
    if (!layout) {
        layout = new QVBoxLayout(contenido);
        contenido->setLayout(layout);
    }

    // LIMPIAR contenido anterior de forma segura
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget())
            item->widget()->deleteLater(); // <<<< seguro
        delete item;
    }

    // Crear el nuevo visor SIN parent "this"
    QTextBrowser* browser = new QTextBrowser(contenido);
    browser->setMarkdown(markdown);
    browser->setOpenExternalLinks(true);

    layout->addWidget(browser);
}
void MainWindow::cargarMarkdownPorDificultad(const QString& dificultad)
{
    qDebug() << "[DEBUG] Se llamó cargarMarkdownPorDificultad con:" << dificultad;
    // Evitar que cargue automáticamente al iniciar la app
    if (primeraSeleccion) {
        primeraSeleccion = false;
        return;
    }

    QString markdown = mongo.obtenerMarkdown(dificultad);

    if (markdown.startsWith("No")) {
        markdown = "# No hay contenido disponible para esta dificultad.\nSeleccione otra opción.";
    }

    cargarMarkdownEnScrollArea(markdown);
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


void MainWindow::enviarCodigo() {
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

    // Compilación y ejecución del código
    ui->TextoSalida->clear();
    ui->pushButton->setEnabled(false);

    emit startProcessing(codigo);

}

void MainWindow::handleNewOutput(const QString &output)
{
    // ¡Esto ahora funcionará!
    ui->TextoSalida->appendPlainText(output);
}

void MainWindow::handleTaskFinished()
{
    ui->pushButton->setEnabled(true);
}