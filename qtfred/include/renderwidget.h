#pragma once
#include <QGLWidget>

///! \bug subsys_to_render should have its own include.
#include "fredrender.h"
namespace fso {
namespace fred {

class RenderWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit RenderWidget(QWidget *parent = 0);

protected:
    void initializeGL();
    void paintGL();

signals:

public slots:

private:
    subsys_to_render Render_subsys;
};

} // namespace fred
} // namespace fso
