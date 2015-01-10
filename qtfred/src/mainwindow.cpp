#include "include/mainwindow.h"
#include "ui_mainwindow.h"

namespace fso {
namespace fred {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

} // namespace fred
} // namespace fso
