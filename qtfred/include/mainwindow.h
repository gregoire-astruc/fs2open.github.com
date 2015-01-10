#ifndef FSO_FRED_MAINWINDOW_H
#define FSO_FRED_MAINWINDOW_H

#include <QMainWindow>

namespace fso {
namespace fred {

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
    Ui::MainWindow *ui;
};


} // namespace fred
} // namespace fso
#endif // FSO_FRED_MAINWINDOW_H
