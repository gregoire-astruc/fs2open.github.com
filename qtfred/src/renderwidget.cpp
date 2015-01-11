#include "renderwidget.h"

#include <array>

#include <QDebug>
#include <QDir>
#include <QKeyEvent>
#include <QTimer>

#include "graphics/gropengl.h"
#include "osapi/osapi.h"
#include "io/key.h"
#include "io/timer.h"
#include "starfield/starfield.h"

namespace fso {
namespace fred {

RenderWidget::RenderWidget(QWidget *parent) :
    QGLWidget(parent)
{
    setFocusPolicy(Qt::ClickFocus);
    qt2fsKeys[Qt::Key_Shift] = KEY_LSHIFT;
    qt2fsKeys[Qt::Key_A] = KEY_A;
    qt2fsKeys[Qt::Key_Z] = KEY_Z;
    qt2fsKeys[Qt::Key_0] = KEY_PAD0;
    qt2fsKeys[Qt::Key_1] = KEY_PAD1;
    qt2fsKeys[Qt::Key_2] = KEY_PAD2;
    qt2fsKeys[Qt::Key_3] = KEY_PAD3;
    qt2fsKeys[Qt::Key_4] = KEY_PAD4;
    qt2fsKeys[Qt::Key_5] = KEY_PAD5;
    qt2fsKeys[Qt::Key_6] = KEY_PAD6;
    qt2fsKeys[Qt::Key_7] = KEY_PAD7;
    qt2fsKeys[Qt::Key_8] = KEY_PAD8;
    qt2fsKeys[Qt::Key_9] = KEY_PAD9;
    qt2fsKeys[Qt::Key_Plus] = KEY_PADPLUS;
    qt2fsKeys[Qt::Key_Minus] = KEY_PADMINUS;

}

void RenderWidget::initializeGL()
{
    qDebug() << "fso::fred::RenderWidget::initializeGL()";
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
    QTimer *updater(new QTimer(this));
    connect(updater, SIGNAL(timeout()), this, SLOT(updateGL()));
    updater->setInterval(30);

    ///! \todo Maybe an option for smooth rendering vs only on events?
    updater->start();
}

void RenderWidget::paintGL()
{
    game_do_frame(-1, 0, 0, -1);
    std::array<bool, MAX_IFFS> iffs;
    iffs.fill(true);
    render_frame(-1, Render_subsys, false, Marking_box(), -1, true, true, &iffs[0], true, true, true, true, false, true, true, true);
}

void RenderWidget::resizeGL(int w, int h)
{
    gr_screen_resize(w, h);
}

void RenderWidget::keyPressEvent(QKeyEvent *key)
{
    if (!qt2fsKeys.count(key->key())) {
        QGLWidget::keyPressEvent(key);
        return;
    }

    key->accept();
    key_mark(qt2fsKeys.at(key->key()), 1, 0);
    //updateGL();
}

void RenderWidget::keyReleaseEvent(QKeyEvent *key)
{
    if (!qt2fsKeys.count(key->key())) {
        QGLWidget::keyPressEvent(key);
        return;
    }

    key->accept();
    qDebug() << "Key" << qt2fsKeys.at(key->key());
    key_mark(qt2fsKeys.at(key->key()), 0, 0);
    //updateGL();
}

void RenderWidget::updateGame()
{
    game_do_frame(-1, 0, 0, -1);
}

} // namespace fred
} // namespace fso
