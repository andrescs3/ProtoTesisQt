
#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "thinning/thinning.h"
#include "limpieza/limpieza.h"
#include "reconstruction/lineReconstruction.h"
#include "cuerpos/Cuerpo.h"
#include "cuerpos/cuerpos.h"
#include "vectorize/vect_catmull.h"
#include "vectorize/vect.h"
#include "vectorize/vect_RDP.h"

using namespace std;
using namespace cv;


std::string resultFileName = "d:/res/lastresult.png";
Mat src;
Mat dst;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


        ui->imageLabel->setBackgroundRole(QPalette::Base);
        ui->imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        ui->imageLabel->setScaledContents(true);


        ui->scrollArea->setBackgroundRole(QPalette::Dark);
        //ui->scrollArea->setWidget(ui->imageLabel);
        createActions();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fitToWindow() {
    //bool fitToWindow = fitToWindowAct->isChecked();
    ui->scrollArea->setWidgetResizable(true);
   // if (!fitToWindow) {
      //  normalSize();
   // }
    //updateActions();
}


/***********SLOTS*******************/
void MainWindow::abrir()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
        if (!fileName.isEmpty()) {
            QImage image(fileName);
            if (image.isNull()) {
                QMessageBox::information(this, tr("Image Viewer"), tr("Cannot load %1.").arg(fileName));
                return;
            }

            // cargamos la imagen seleccionada
            src = imread(fileName.toStdString());

            // Convertimos según el formato de la imagen
            int nchannels = src.channels();
            if (nchannels == 3) {
                cvtColor(src, src, CV_BGR2GRAY); // a escala de grises
            }


            ui->imageLabel->setPixmap(QPixmap::fromImage(image));
            scaleFactor = 1.0;

            //printAct->setEnabled(true);
            //fitToWindowAct->setEnabled(true);
            //updateActions();

            //if (!fitToWindowAct->isChecked())
            ui->imageLabel->adjustSize();
        }
}


void MainWindow::createActions()
{
    connect(ui->actionAbrir_Imagen, SIGNAL(triggered()), this, SLOT(abrir()));

}

void MainWindow::ejecutar()
{
    if(ui->chkLimpiarPixeles->checkState())
    {
        cleanIsolated();
        cleanIsolatedLines();
    }
    if(ui->chkLimpiarCuerpos->checkState())
    {
        cleanBodies();
    }
    if(ui->chkAdelgazar->checkState())
    {
        thinningGuoHall();
    }
    if(ui->chkReconstruccion->checkState())
    {
        lineReconstruction();
    }
    if(ui->chkVectorizacion->checkState())
    {
        vectorize();
    }

}

void MainWindow::on_btnEjecutar_clicked()
{
    ejecutar();
}

/* Muestra la imagen de resultado en pantalla */
void MainWindow::showResultImage() {
    // Cargamos el último resultado como una QImage
    QString qstr = QString::fromStdString(resultFileName);
    QImage resultImage(qstr);

    // Mostramos la imagen
    ui->imageLabel->setPixmap(QPixmap::fromImage(resultImage));
    scaleFactor = 1.0;
   // printAct->setEnabled(true);
    //fitToWindowAct->setEnabled(true);
    //updateActions();
    //if (!fitToWindowAct->isChecked())

}

/* Guarda la imagen de resultado en disco */
void MainWindow::saveResultImage() {
    bool resultado = imwrite(resultFileName, dst);
    if (!resultado) {
        QMessageBox msgBox(QMessageBox::Warning, tr("No se pudo guardar la imagen"), tr("No se pudo guardar la imagen de resultado en la ruta configurada."), 0, this);
        msgBox.exec();
    }
}

void  MainWindow::thinningGuoHall() {
    execThinningGuoHall(src, dst);
    src = dst.clone();
    saveResultImage();
    showResultImage();
}

void MainWindow::lineReconstruction() {
    for(int k=0; k<1; k++) {
        execLineReconstruction(src, dst, 50 + k*5);
        src = dst.clone();
        saveResultImage();
        showResultImage();
    }
}

void  MainWindow::thinningZhang() {
    execThinningZhang(src, dst);
    src = dst.clone();
    saveResultImage();
    showResultImage();
}

void MainWindow::cleanIsolated() {
    limpia::exec_limpiarPxAislados(src, dst);
    src = dst.clone();
    saveResultImage();
    showResultImage();
}

void MainWindow::cleanIsolatedLines() {
    limpia::exec_limpiarTrazosAislados(src, dst);
    src = dst;
    saveResultImage();
    showResultImage();
}

void MainWindow::cleanBodies()
{
    cuerpos::exec_limpiarCuerpos(src, dst);
    src = dst;
    saveResultImage();
    showResultImage();

}

void MainWindow::vectorize()
{
    Mat imgGrises, imgBinaria;
    //cvtColor(src, imgGrises, CV_BGR2GRAY); // a escala de grises
    threshold(src, imgBinaria, 200, 255, CV_THRESH_BINARY); // filtro de umbral para obtener la versión binaria
    copyMakeBorder(imgBinaria, imgBinaria, 1, 1, 1, 1, BORDER_CONSTANT, 255); // agregamos borde de 1px
    vect::execVectorize(imgBinaria, "vectorizado", 1, 1);
    src = imgBinaria;
    saveResultImage();
    showResultImage();
}
