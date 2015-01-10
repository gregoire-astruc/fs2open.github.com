#include <QApplication>
#include <QDir>
#include "globalincs/globals.h"
#include "globalincs/pstypes.h"
#include "io/timer.h"
#include "graphics/2d.h"
#include "osapi/osregistry.h"
#include "osapi/osapi.h"
#include "cfile/cfile.h"
#include "object/object.h"
#include "localization/localize.h"
#include "renderwidget.h"

// Globals needed by the engine when built in 'FRED' mode.
int Fred_running = 1;
char Fred_callsigns[MAX_SHIPS][NAME_LENGTH+1];
char Fred_alt_names[MAX_SHIPS][NAME_LENGTH+1];
int Show_cpu = 0;

// Empty functions to make fred link with the sexp_mission_set_subspace
void game_start_subspace_ambient_sound() {}
void game_stop_subspace_ambient_sound() {}

namespace {
void os_set_window_from_hwnd(WId handle)
{
    if (!SDL_WasInit(SDL_INIT_VIDEO))
        SDL_InitSubSystem(SDL_INIT_VIDEO);

    if (SDL_GL_LoadLibrary(NULL) < 0)
            Error(LOCATION, "Failed to load OpenGL library: %s!", SDL_GetError());


    SDL_Window* window = SDL_CreateWindowFrom((void*) handle);

    if (os_get_window())
        SDL_DestroyWindow(os_get_window());
    os_set_window(window);
}
}



int main(int argc, char *argv[])
{
    //outwnd_init();
    QApplication app(argc, argv);
    fso::fred::RenderWidget w;


    if (!vm_init(24*1024*1024)) {
        qFatal("Unable to allocate VM.");
        return -1;
    }

    //! \bug This leaks.
    char *Fred_base_dir = strdup(QDir::current().absolutePath().toStdString().c_str());
    os_init_registry_stuff(Osreg_company_name, Osreg_app_name, NULL);
    timer_init();
    cfile_chdir(Fred_base_dir);

    // d'oh
    if(cfile_init(Fred_base_dir)){
        qFatal("Unable to cfile init.");
        return -2;
    }

    lcl_init(FS2_OPEN_DEFAULT_LANGUAGE);
    gr_init(GR_OPENGL, 640, 480, 32);
    os_set_window_from_hwnd(w.effectiveWinId());
    obj_init();

    //
    w.show();


    return app.exec();
}
