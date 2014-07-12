#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    double scaleFactor;
    Ui::MainWindow *ui;
    void createActions();
    void createMenus();
    void fitToWindow();

private slots:
    void abrir();
    void ejecutar();
    void on_btnEjecutar_clicked();
    void showResultImage();
    void saveResultImage();
    void thinningGuoHall();
    void thinningZhang();
    void lineReconstruction();
    void cleanIsolated();
    void cleanIsolatedLines();
    void cleanBodies();
};

#endif // MAINWINDOW_H
