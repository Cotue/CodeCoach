#include "mainWindow.h"
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
#include "Compiler.h"
#include "ComplexityAnalyzer.h"

void MainWindow::on_btnComplejidad_clicked()
{
    if (!m_CodeEditor) return;

    QString codigo = m_CodeEditor->toPlainText();

    if (codigo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "No hay código para analizar.");
        return;
    }

    auto result = ComplexityAnalyzer::analyze(codigo);

    QString reporte;
    reporte += "=== Complejidad Temporal ===\n";
    reporte += result.timeComplexity + "\n\n";

    reporte += "=== Complejidad Espacial ===\n";
    reporte += result.spaceComplexity + "\n\n";

    reporte += QString("For loops: %1\n").arg(result.forLoops);
    reporte += QString("Bucles anidados: %1\n").arg(result.nestedLoops);
    reporte += QString("While loops: %1\n").arg(result.whileLoops);
    reporte += QString("Recursión detectada: %1\n").arg(result.recursionCount);
    reporte += QString("Asignaciones dinámicas: %1\n").arg(result.dynamicAllocations);
    reporte += QString("Uso de vector: %1\n").arg(result.vectorAllocations);

    // Mostrar en tu QTextEdit/QPlainTextEdit
    ui->TextoSalida->appendPlainText(reporte);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Para el LLM
    m_CodeEditor = nullptr;
    ui->setupUi(this);
    QString rutaArchivo = QDir(QCoreApplication::applicationDirPath())
                      .filePath("../markdowns/prueba.md");
    cargarMarkdownEnScrollArea(rutaArchivo);
    crearEditorEnScrollArea2();
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::enviarCodigo);

    connect(ui->btnAyuda, &QPushButton::clicked, this, &MainWindow::on_btnPedirPista);

    // Configuración del hilo y el compilador

    // Compilador
    compiler = new Compiler;

    // Mover el compilador al hilo
    compiler->moveToThread(&compilerThread);
    // Conectar las señales

    // Conexión para iniciar el trabajo (GUI -> Worker)
    connect(this, &MainWindow::startProcessing, compiler, &Compiler::processCode);

    // Conexiones para recibir resultados (Worker -> GUI)
        connect(compiler, &Compiler::newOutput, this, &MainWindow::handleNewOutput);
        connect(compiler, &Compiler::taskFinished, this, &MainWindow::handleTaskFinished);

    // Conexión para limpiar el worker cuando el hilo termine
    connect(&compilerThread, &QThread::finished, compiler, &QObject::deleteLater);

    // Se inicia el hilo. Ahora está "dormido" esperando señales.
    compilerThread.start();

    m_llmService = new LLMService(this);

    // --- Servicio LLM con Gemini ---
    m_llmService = new LLMService(this);

    connect(m_llmService, &LLMService::pistaRecibida, this, [this](const QString &pista){
        // Restaurar cursor normal
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::information(this, "Tutor de IA (Gemini)", pista);
    });

    connect(m_llmService, &LLMService::errorOcurrido, this, [this](const QString &error){
        this->setCursor(Qt::ArrowCursor);
        QMessageBox::warning(this, "Error de IA", error);
    });
}


MainWindow::~MainWindow()
{
    compilerThread.quit();
    compilerThread.wait();
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
    browser->setMarkdown(markdown);
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
    m_CodeEditor = new CodeEditor(this);

    new CppHighlighter(m_CodeEditor->document());
    m_CodeEditor->setPlaceholderText("// Escribe tu código aquí...\n");

    // Obtener el widget contenido dentro del scrollArea
    QWidget* contenido = ui->scrollArea_2->widget();
    if (!contenido) {
        qWarning("scrollArea_2 no tiene contenido interno asignado desde Qt Designer");
        contenido = new QWidget();
        ui->scrollArea_2->setWidget(contenido);
        ui->scrollArea_2->setWidgetResizable(true);
        return;
    }

    //  Obtener el layout vertical ya existente (verticalLayout_2)
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(contenido->layout());
    if (!layout) {
        layout = new QVBoxLayout(contenido);
        contenido->setLayout(layout);
    }

    // El layout ya tiene el pushButton del .ui, así que solo agregamos el editor antes de él
    layout->insertWidget(0, m_CodeEditor);  // Insertar el editor arriba del botón

    // (opcional) Margen visual
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(6);
}


void MainWindow::enviarCodigo() {
    if (!m_CodeEditor) {
        QMessageBox::critical(this, "Error", "El editor de código no se ha inicializado.");
        return;
    }

    QString codigo = m_CodeEditor->toPlainText();

    if (codigo.isEmpty()) {
        QMessageBox::information(this, "Aviso", "El código está vacío.");
        return;
    }

    // 2. Limpieza antes de nueva ejecución
    ui->TextoSalida->clear();
    m_ultimaSalida.clear(); // ¡IMPORTANTE! Limpiar el buffer de la IA
    ui->pushButton->setEnabled(false);

    emit startProcessing(codigo);
}

void MainWindow::handleNewOutput(const QString &output)
{
    // Mostrar en la GUI
    ui->TextoSalida->appendPlainText(output);
    // Acumular salidas para la IA
    m_ultimaSalida += output;
}

void MainWindow::handleTaskFinished()
{
    ui->pushButton->setEnabled(true);
}

void MainWindow::on_btnPedirPista() {
    if (!m_CodeEditor) return;

    QString codigo = m_CodeEditor->toPlainText();

    if (m_ultimaSalida.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Primero debes compilar el código para que la IA pueda analizar los errores.");
        return;
    }

    // Indicador visual de carga (Cursor de reloj de arena)
    this->setCursor(Qt::WaitCursor);

    // Llamada al servicio
    m_llmService->solicitarPista(codigo, "Analiza los errores adjuntos", m_ultimaSalida);
}

