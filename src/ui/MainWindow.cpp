#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "../canvas/CanvasWidget.h"
#include "panels/ToolPanel.h"
#include "panels/ColorPanel.h"
#include "panels/LayerPanel.h"
#include "dialogs/NewProjectDialog.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    m_canvasWidget = new CanvasWidget(this);
    setCentralWidget(m_canvasWidget);

    // Setup Dock Panels
    m_toolPanel = new ToolPanel(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_toolPanel);

    m_colorPanel = new ColorPanel(this);
    addDockWidget(Qt::RightDockWidgetArea, m_colorPanel);

    m_layerPanel = new LayerPanel(m_canvasWidget, this);
    addDockWidget(Qt::RightDockWidgetArea, m_layerPanel);

    // Connect Signals
    connect(m_toolPanel, &ToolPanel::penToolSelected, this, &MainWindow::setPenTool);
    connect(m_toolPanel, &ToolPanel::eraserToolSelected, this, &MainWindow::setEraserTool);
    connect(m_toolPanel, &ToolPanel::fillToolSelected, this, &MainWindow::setFillTool);
    connect(m_toolPanel, &ToolPanel::eyedropperToolSelected, this, &MainWindow::setEyedropperTool);
    connect(m_toolPanel, &ToolPanel::lineToolSelected, this, &MainWindow::setLineTool);
    
    connect(m_colorPanel, &ColorPanel::colorChanged, m_canvasWidget, &CanvasWidget::setBrushColor);
    connect(m_colorPanel, &ColorPanel::brushSizeChanged, m_canvasWidget, &CanvasWidget::setBrushSize);
    
    // Connect reverse color sync (Eyedropper -> ColorPanel)
    connect(m_canvasWidget, &CanvasWidget::brushColorChanged, m_colorPanel, &ColorPanel::setColor);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    NewProjectDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        int w = dialog.getCanvasWidth();
        int h = dialog.getCanvasHeight();
        m_canvasWidget->setCanvasSize(w, h);
    }
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Files (*.png);;All Files (*)"));
    if (!fileName.isEmpty()) {
        // Linux dialogs don't always auto-append the extension.
        // QImage::save() will fail if it cannot deduce the image format from the extension.
        if (!fileName.endsWith(".png", Qt::CaseInsensitive)) {
            fileName += ".png";
        }
        
        if (!m_canvasWidget->saveImage(fileName)) {
            QMessageBox::warning(this, tr("Error"), tr("Failed to save image."));
        }
    }
}

void MainWindow::on_actionClear_triggered()
{
    m_canvasWidget->clearCanvas();
}

void MainWindow::setPenTool()
{
    m_canvasWidget->setTool(CanvasWidget::Pen);
}

void MainWindow::setEraserTool()
{
    m_canvasWidget->setTool(CanvasWidget::Eraser);
}

void MainWindow::setFillTool()
{
    m_canvasWidget->setTool(CanvasWidget::Fill);
}

void MainWindow::setEyedropperTool()
{
    m_canvasWidget->setTool(CanvasWidget::Eyedropper);
}

void MainWindow::setLineTool()
{
    m_canvasWidget->setTool(CanvasWidget::Line);
}
