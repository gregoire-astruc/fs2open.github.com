#include "renderwidget.h"

#include <QDebug>
#include <QDir>

#include "graphics/gropengl.h"
#include "osapi/osapi.h"
#include "io/timer.h"

namespace fso {
namespace fred {

RenderWidget::RenderWidget(QWidget *parent) :
    QGLWidget(parent)
{
}

void RenderWidget::initializeGL()
{
    qDebug() << "fso::fred::RenderWidget::initializeGL()";
    fred_render_init();
}

void RenderWidget::paintGL()
{
    game_do_frame(-1, 0, 0, -1);
    render_frame(-1, Render_subsys, false, Marking_box(), -1, true, true, {}, true, true, true, true, false, true, true, true);
}

} // namespace fred
} // namespace fso
