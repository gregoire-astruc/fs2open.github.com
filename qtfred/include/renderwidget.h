#pragma once
#include <unordered_map>
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
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;

signals:

private slots:
    void updateGame();

private:
    subsys_to_render Render_subsys;
    std::unordered_map<int, int> qt2fsKeys;
};

} // namespace fred
} // namespace fso
