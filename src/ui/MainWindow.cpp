#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "../canvas/CanvasWidget.h"
#include "panels/ToolPanel.h"
#include "panels/ToolOptionsBar.h"
#include "panels/ColorPanel.h"
#include "panels/LayerPanel.h"
#include "dialogs/NewProjectDialog.h"
#include "dialogs/CanvasSizeDialog.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QApplication>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_fgColor(Qt::black)
    , m_bgColor(Qt::white)
{
    ui->setupUi(this);

    ui->actionLowMemoryMode->setCheckable(true);

    m_canvasWidget = new CanvasWidget(this);
    setCentralWidget(m_canvasWidget);

    m_toolOptionsBar = new ToolOptionsBar(m_canvasWidget, this);
    addToolBar(Qt::TopToolBarArea, m_toolOptionsBar);

    m_toolPanel = new ToolPanel(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_toolPanel);

    m_colorPanel = new ColorPanel(this);
    addDockWidget(Qt::RightDockWidgetArea, m_colorPanel);

    m_layerPanel = new LayerPanel(m_canvasWidget, this);
    addDockWidget(Qt::RightDockWidgetArea, m_layerPanel);

    tabifyDockWidget(m_colorPanel, m_layerPanel);
    m_colorPanel->raise();

    setupStatusBar();

    connect(m_toolPanel, &ToolPanel::toolSelected, this, &MainWindow::setTool);
    connect(m_toolPanel, &ToolPanel::swapColors, this, &MainWindow::swapColors);

    connect(m_colorPanel, &ColorPanel::colorChanged, m_canvasWidget, &CanvasWidget::setBrushColor);
    connect(m_colorPanel, &ColorPanel::colorChanged, [this](const QColor &color) {
        m_fgColor = color;
        m_toolPanel->updateSwatches(m_fgColor, m_bgColor);
    });

    connect(m_canvasWidget, &CanvasWidget::brushColorChanged, m_colorPanel, &ColorPanel::setColor);
    connect(m_canvasWidget, &CanvasWidget::zoomChanged, this, &MainWindow::updateStatusBarZoom);
    connect(m_canvasWidget, &CanvasWidget::historyChanged, this, &MainWindow::updateUndoRedoActions);
    connect(m_canvasWidget, &CanvasWidget::lowMemoryModeChanged, this, &MainWindow::syncLowMemoryAction);

    updateUndoRedoActions();
    m_toolPanel->updateSwatches(m_fgColor, m_bgColor);
    setTool(0);

    // Document-dirty tracking. documentEdited fires for every real content
    // change (coarse edit, patch commit, undo/redo, project load). It does
    // NOT fire for low-memory idle release/restore — those mutate the
    // history storage but are not user edits.
    connect(m_canvasWidget, &CanvasWidget::documentEdited,
            this, &MainWindow::updateDirtyFlag);
    updateDirtyFlag();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupStatusBar()
{
    m_zoomLabel = new QLabel("100%");
    m_zoomLabel->setMinimumWidth(50);
    m_zoomLabel->setAlignment(Qt::AlignCenter);

    m_sizeLabel = new QLabel("800 × 600 px");
    m_sizeLabel->setMinimumWidth(100);
    m_sizeLabel->setAlignment(Qt::AlignCenter);

    m_toolLabel = new QLabel("Brush Tool");
    m_toolLabel->setMinimumWidth(150);

    ui->statusbar->addPermanentWidget(m_zoomLabel);
    ui->statusbar->addPermanentWidget(m_sizeLabel);
    ui->statusbar->addWidget(m_toolLabel);
}

void MainWindow::updateUndoRedoActions()
{
    ui->actionUndo->setEnabled(m_canvasWidget->canUndo());
    ui->actionRedo->setEnabled(m_canvasWidget->canRedo());
}

void MainWindow::setTool(int toolType)
{
    m_canvasWidget->setTool(static_cast<CanvasWidget::ToolType>(toolType));
    m_toolOptionsBar->setToolType(toolType);

    const QString toolNames[] = {
        "Brush Tool", "Eraser Tool", "Paint Bucket Tool", "Eyedropper Tool", "Line Tool",
        "Move Tool", "Rectangular Marquee Tool", "Elliptical Marquee Tool", "Lasso Tool",
        "Horizontal Type Tool", "Zoom Tool", "Hand Tool"
    };

    if (toolType >= 0 && toolType <= 11) {
        m_toolLabel->setText(toolNames[toolType]);
    }
}

void MainWindow::swapColors()
{
    std::swap(m_fgColor, m_bgColor);
    m_colorPanel->setColor(m_fgColor);
    m_toolPanel->updateSwatches(m_fgColor, m_bgColor);
}

void MainWindow::updateStatusBarZoom(double zoom)
{
    m_zoomLabel->setText(QString::number(int(zoom * 100)) + "%");
    const QSize size = m_canvasWidget->canvasSize();
    m_sizeLabel->setText(QString("%1 × %2 px").arg(size.width()).arg(size.height()));
}

void MainWindow::on_actionNew_triggered()
{
    NewProjectDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_canvasWidget->newCanvas(dialog.getCanvasWidth(), dialog.getCanvasHeight());
        updateStatusBarZoom(m_canvasWidget->zoomLevel());
        updateUndoRedoActions();
        // Fresh canvas → no file path, cursor + patch depths back to zero.
        m_currentFilePath.clear();
        resetSavedCursor();
        updateDirtyFlag();
    }
}

void MainWindow::on_actionOpen_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open"),
        "",
        tr("Brusher Projects (*.brusher);;Images (*.png *.jpg *.jpeg *.bmp);;All Files (*)"));

    if (!fileName.isEmpty()) {
        if (!m_canvasWidget->openImage(fileName)) {
            QMessageBox::warning(this, tr("Error"), tr("Failed to open file."));
        }
        updateStatusBarZoom(m_canvasWidget->zoomLevel());
        updateUndoRedoActions();

        // Treat the just-loaded document as the saved baseline. The loaded
        // .brusher restores its persisted cursor; flat images land at index
        // 0 with empty patch stacks. Either way, the document is clean.
        m_currentFilePath = fileName;
        resetSavedCursor();
        updateDirtyFlag();
    }
}

void MainWindow::on_actionSave_triggered()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save Image As"), "", tr("PNG Files (*.png);;All Files (*)"));

    if (fileName.isEmpty()) {
        return;
    }

    QString path = fileName;
    if (!path.endsWith(".png", Qt::CaseInsensitive)) {
        path += ".png";
    }
    if (!m_canvasWidget->saveImage(path)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to save image."));
        return;
    }

    // Save succeeded → this path is now the saved baseline. Format is
    // PNG, so it should NOT be saved as a .brusher project later.
    m_currentFilePath = path;
    captureSavedCursor();
    updateDirtyFlag();
}

void MainWindow::on_actionSaveProject_triggered()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save Project"), "", tr("Brusher Projects (*.brusher);;All Files (*)"));

    if (fileName.isEmpty()) {
        return;
    }

    QString path = fileName;
    if (!path.endsWith(".brusher", Qt::CaseInsensitive)) {
        path += ".brusher";
    }
    if (!m_canvasWidget->saveProject(path)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to save project."));
        return;
    }

    m_currentFilePath = path;
    captureSavedCursor();
    updateDirtyFlag();
}

void MainWindow::on_actionExport_triggered()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Export Image"),
        "",
        tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp);;All Files (*)"));

    if (!fileName.isEmpty() && !m_canvasWidget->exportImage(fileName)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to export image."));
    }
}

void MainWindow::on_actionExit_triggered()
{
    // Route File > Exit and Ctrl+Q through closeEvent so the unsaved-changes
    // prompt covers every quit path (window X, Alt+F4, menu Exit).
    close();
}

void MainWindow::on_actionUndo_triggered()
{
    m_canvasWidget->undo();
    updateUndoRedoActions();
}

void MainWindow::on_actionRedo_triggered()
{
    m_canvasWidget->redo();
    updateUndoRedoActions();
}


void MainWindow::on_actionClear_triggered()
{
    m_canvasWidget->clearCanvas();
    updateUndoRedoActions();
}

void MainWindow::on_actionCanvasSize_triggered()
{
    const QSize current = m_canvasWidget->canvasSize();
    CanvasSizeDialog dialog(current.width(), current.height(), this);
    if (dialog.exec() == QDialog::Accepted) {
        m_canvasWidget->resizeCanvas(dialog.getCanvasWidth(), dialog.getCanvasHeight());
        updateStatusBarZoom(m_canvasWidget->zoomLevel());
        updateUndoRedoActions();
    }
}

void MainWindow::on_actionRotateCW_triggered()
{
    m_canvasWidget->rotateCanvas(true);
    updateStatusBarZoom(m_canvasWidget->zoomLevel());
    updateUndoRedoActions();
}

void MainWindow::on_actionRotateCCW_triggered()
{
    m_canvasWidget->rotateCanvas(false);
    updateStatusBarZoom(m_canvasWidget->zoomLevel());
    updateUndoRedoActions();
}

void MainWindow::on_actionFlipHorizontal_triggered()
{
    m_canvasWidget->flipCanvas(true);
    updateUndoRedoActions();
}

void MainWindow::on_actionFlipVertical_triggered()
{
    m_canvasWidget->flipCanvas(false);
    updateUndoRedoActions();
}

void MainWindow::on_actionCrop_triggered()
{
    if (!m_canvasWidget->hasSelection()) {
        QMessageBox::information(this, tr("Crop"), tr("Create a selection before cropping."));
        return;
    }
    m_canvasWidget->cropToSelection();
    updateStatusBarZoom(m_canvasWidget->zoomLevel());
    updateUndoRedoActions();
}

void MainWindow::on_actionBlur_triggered()
{
    m_canvasWidget->applyBlurFilter(2);
    updateUndoRedoActions();
}

void MainWindow::on_actionSharpen_triggered()
{
    m_canvasWidget->applySharpenFilter(1.0f);
    updateUndoRedoActions();
}

void MainWindow::on_actionNewLayer_triggered()
{
    m_canvasWidget->addLayer("New Layer");
    updateUndoRedoActions();
}

void MainWindow::on_actionDuplicateLayer_triggered()
{
    const int active = m_canvasWidget->getActiveLayerIndex();
    if (active >= 0) {
        m_canvasWidget->duplicateLayer(active);
        updateUndoRedoActions();
    }
}

void MainWindow::on_actionDeleteLayer_triggered()
{
    const int active = m_canvasWidget->getActiveLayerIndex();
    if (active >= 0) {
        m_canvasWidget->removeLayer(active);
        updateUndoRedoActions();
    }
}

void MainWindow::on_actionFlattenImage_triggered()
{
    m_canvasWidget->flattenAllLayers();
    updateUndoRedoActions();
}

void MainWindow::on_actionZoomIn_triggered()
{
    m_canvasWidget->zoomIn();
}

void MainWindow::on_actionZoomOut_triggered()
{
    m_canvasWidget->zoomOut();
}

void MainWindow::on_actionFitToWindow_triggered()
{
    m_canvasWidget->fitToWindow();
}

void MainWindow::on_actionActualSize_triggered()
{
    m_canvasWidget->actualSize();
}

void MainWindow::on_actionLowMemoryMode_triggered(bool checked)
{
    m_canvasWidget->setLowMemoryMode(checked);
}

void MainWindow::syncLowMemoryAction()
{
    QSignalBlocker blocker(ui->actionLowMemoryMode);
    ui->actionLowMemoryMode->setChecked(m_canvasWidget->isLowMemoryMode());
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);

    if (!m_canvasWidget) {
        return;
    }

    switch (event->type()) {
        case QEvent::WindowStateChange: {
            // Fires when the user minimizes, restores, maximizes, etc.
            if (m_canvasWidget->isLowMemoryMode() && isMinimized()) {
                m_canvasWidget->releaseIdleResources();
            } else if (!isMinimized() && m_canvasWidget->isResourcesReleased()) {
                m_canvasWidget->restoreIdleResources();
            }
            break;
        }
        case QEvent::WindowActivate: {
            if (m_canvasWidget->isResourcesReleased()) {
                m_canvasWidget->restoreIdleResources();
            }
            break;
        }
        case QEvent::WindowDeactivate: {
            if (m_canvasWidget->isLowMemoryMode()) {
                m_canvasWidget->releaseIdleResources();
            }
            break;
        }
        default:
            break;
    }
}

void MainWindow::captureSavedCursor()
{
    // Snapshot the history cursor + patch-stack depths so we can later
    // detect whether the document has drifted from the last save/load.
    m_savedHistoryIndex = m_canvasWidget->currentHistoryIndex();
    m_savedPatchUndoDepth = m_canvasWidget->patchUndoDepth();
    m_savedPatchRedoDepth = m_canvasWidget->patchRedoDepth();
}

void MainWindow::resetSavedCursor()
{
    // A fresh document (New) or a freshly loaded one (Open) is clean by
    // definition; reset to the current cursor so the dirty check matches.
    captureSavedCursor();
}

void MainWindow::updateDirtyFlag()
{
    if (!m_canvasWidget) {
        return;
    }

    const bool dirty =
        m_canvasWidget->currentHistoryIndex() != m_savedHistoryIndex
        || m_canvasWidget->patchUndoDepth() != m_savedPatchUndoDepth
        || m_canvasWidget->patchRedoDepth() != m_savedPatchRedoDepth;

    setWindowModified(dirty);

    // Mirror the standard Qt pattern: title shows the file name (or
    // "Untitled") followed by the `[*]` placeholder that setWindowModified
    // expands to either "" or "*".
    QString title;
    if (m_currentFilePath.isEmpty()) {
        title = tr("Untitled");
    } else {
        title = QFileInfo(m_currentFilePath).fileName();
    }
    title += "[*] - Brusher";
    setWindowTitle(title);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSaveBeforeClose()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::maybeSaveBeforeClose()
{
    // Clean document (or no document loaded yet) → nothing to ask.
    if (!isWindowModified()) {
        return true;
    }

    QMessageBox box(this);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle(tr("Unsaved Changes"));
    box.setText(tr("The document has unsaved changes."));
    box.setInformativeText(tr("Save changes before closing?"));
    box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    box.setDefaultButton(QMessageBox::Save);
    switch (box.exec()) {
        case QMessageBox::Save:   return saveCurrentDocument();
        case QMessageBox::Discard: return true;
        case QMessageBox::Cancel:
        default:                   return false;
    }
}

bool MainWindow::saveCurrentDocument()
{
    // Match the format of the last save/load: a .brusher path was opened
    // from or saved to a project file, so saving again should keep all
    // layer information. Anything else (including Untitled) defaults to
    // PNG via the standard Save dialog.
    const bool saveAsProject =
        !m_currentFilePath.isEmpty() &&
        m_currentFilePath.endsWith(".brusher", Qt::CaseInsensitive);

    if (saveAsProject) {
        QString path = m_currentFilePath;
        if (!m_canvasWidget->saveProject(path)) {
            QMessageBox::warning(this, tr("Error"), tr("Failed to save project."));
            return false;
        }
    } else {
        // Empty path → ask where to save (Save As PNG). Non-empty but
        // non-.brusher path → overwrite that PNG in place.
        QString path;
        if (m_currentFilePath.isEmpty()) {
            const QString fileName = QFileDialog::getSaveFileName(
                this, tr("Save Image As"), "", tr("PNG Files (*.png);;All Files (*)"));
            if (fileName.isEmpty()) {
                return false; // user cancelled the save dialog
            }
            path = fileName;
            if (!path.endsWith(".png", Qt::CaseInsensitive)) {
                path += ".png";
            }
        } else {
            path = m_currentFilePath;
        }

        if (!m_canvasWidget->saveImage(path)) {
            QMessageBox::warning(this, tr("Error"), tr("Failed to save image."));
            return false;
        }
        m_currentFilePath = path;
    }

    captureSavedCursor();
    updateDirtyFlag();
    return true;
}
