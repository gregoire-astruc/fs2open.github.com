#pragma once
#include <memory>
#include <QMainWindow>

namespace fso {
namespace fred {

class Editor;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setEditor(std::shared_ptr<Editor>);

public slots:
    void loadMission();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Editor> fred;
};


} // namespace fred
} // namespace fso
