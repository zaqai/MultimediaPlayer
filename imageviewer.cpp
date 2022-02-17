#include "imageviewer.h"
#include<QDebug>
#include <QApplication>
#include <QClipboard>
#include <QColorSpace>
#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QStandardPaths>
#include <QStatusBar>
#include <QWheelEvent>



ImageViewer::ImageViewer(QWidget *parent)
    : QMainWindow(parent), imageLabel(new QLabel)
    , scrollArea(new QScrollArea)
{
    this->resize(800,600);
    setWindowTitle("imageViewer");
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //imageLabel得到尽可能多的空间
    imageLabel->setScaledContents(true);//允许缩放图片以填满imageLabel

    scrollArea->setWidget(imageLabel);

    setCentralWidget(scrollArea);

    createActions();

}

bool ImageViewer::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        return false;
    }
    setImage(newImage);

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
            .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}

void ImageViewer::setImage(const QImage &newImage)
{
    image = newImage;
    if (image.colorSpace().isValid())
        image.convertToColorSpace(QColorSpace::SRgb);
    imageLabel->setPixmap(QPixmap::fromImage(image));

    scaleFactor = 1.0;

    scrollArea->setVisible(true);

    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();
}



bool ImageViewer::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}


static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);

        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
        //如果picturesLocations为空返回当前exe文件所在路径，否则返回图片所在路径
    }

    QStringList mimeTypeFilters;

    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
            ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    //判断是打开文件还是保存文件

    for (const QByteArray &mimeTypeName : supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);

    dialog.setMimeTypeFilters(mimeTypeFilters);
    //    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void ImageViewer::open()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}


void ImageViewer::save()
{
    QFileDialog dialog(this, tr("Save File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}


void ImageViewer::copy()
{

    QGuiApplication::clipboard()->setImage(image);

}


static QImage clipboardImage()
{
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}


void ImageViewer::paste()
{

    const QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        statusBar()->showMessage(tr("No image in clipboard"));
    } else {
        setImage(newImage);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
                .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        statusBar()->showMessage(message);
    }
}


void ImageViewer::zoomIn()

{
    scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}


void ImageViewer::normalSize()

{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()

{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}

void ImageViewer::createActions()

{
    QMenu *fileMenu = menuBar()->addMenu(tr("File"));

    QAction *openAct = fileMenu->addAction(tr("Open"), this, &ImageViewer::open);
    openAct->setShortcut(QKeySequence::Open);//快捷键

    saveAct = fileMenu->addAction(tr("Save"), this, &ImageViewer::save);
    saveAct->setEnabled(false);
    saveAct->setShortcut(tr("Ctrl+S"));

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("Exit"), this, &ImageViewer::onQuit);
    exitAct->setShortcut(tr("Ctrl+Q"));

    QMenu *editMenu = menuBar()->addMenu(tr("Edit"));

    copyAct = editMenu->addAction(tr("Copy"), this, &ImageViewer::copy);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setEnabled(false);

    QAction *pasteAct = editMenu->addAction(tr("Paste"), this, &ImageViewer::paste);
    pasteAct->setShortcut(QKeySequence::Paste);

    QMenu *viewMenu = menuBar()->addMenu(tr("View"));

    zoomInAct = viewMenu->addAction(tr("Zoom In"), this, &ImageViewer::zoomIn);
    zoomInAct->setShortcut(tr("ctrl+="));
    zoomInAct->setEnabled(false);



    zoomOutAct = viewMenu->addAction(tr("Zoom Out"), this, &ImageViewer::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("Normal Size"), this, &ImageViewer::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+N"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("Fit to Window"), this, &ImageViewer::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
}

void ImageViewer::updateActions()

{
    saveAct->setEnabled(!image.isNull());
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
    if ( QApplication::keyboardModifiers () == Qt::ControlModifier)
    {
        if (event->delta() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
    }
}


void ImageViewer::scaleImage(double factor)

{

    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::onQuit()
{
    emit mySignal();
    QWidget::close();
}


void ImageViewer::sendSlot()
{
    emit mySignal();
}
