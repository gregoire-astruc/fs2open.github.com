#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include "mission/missionparse.h"
#include "object/object.h"
#include "starfield/starfield.h"
#include "fredrender.h"

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

void MainWindow::loadMission()
{
    qDebug() << "Loading from directory:" << QDir::currentPath();
    QString pathName = QFileDialog::getOpenFileName(this,
                                                    tr("Load mission"),
                                                    QString(),
                                                    tr("FS2 missions (*.fs2)"));

    if (pathName.isEmpty())
        return;

    statusBar()->showMessage(tr("Loading mission %1").arg(pathName));

    ///! \todo Below belongs to a dedicated 'editor' class.
    if (parse_main(pathName.toStdString().c_str())) {
        QMessageBox::critical(this, tr("Unable to load mission"), tr("Loading failed."));
        return;
    }

    obj_merge_created_list();

    view_orient = Parse_viewer_orient;
    view_pos = Parse_viewer_pos;
    stars_post_level_init();
    fred_render_init();
    fred_render_init();
    int physics_speed = 100;
    int physics_rot = 20;
    physics_init(&view_physics);
    view_physics.max_vel.xyz.x *= physics_speed / 3.0f;
    view_physics.max_vel.xyz.y *= physics_speed / 3.0f;
    view_physics.max_vel.xyz.z *= physics_speed / 3.0f;
    view_physics.max_rear_vel *= physics_speed / 3.0f;
    view_physics.max_rotvel.xyz.x *= physics_rot / 30.0f;
    view_physics.max_rotvel.xyz.y *= physics_rot / 30.0f;
    view_physics.max_rotvel.xyz.z *= physics_rot / 30.0f;
    view_physics.flags |= PF_ACCELERATES | PF_SLIDE_ENABLED;
    ui->centralwidget->updateGL();
    statusBar()->showMessage(tr("Units = %1 meters").arg(The_grid->square_size));
}

} // namespace fred
} // namespace fso
