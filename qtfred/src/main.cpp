#include <vector>
#include <utility>
#include <functional>

#include <stdlib.h>

#include <QApplication>
#include <QDir>
#include <QSplashScreen>

#include "globalincs/globals.h"
#include "globalincs/pstypes.h"
#include "io/timer.h"
#include "io/key.h"
#include "io/mouse.h"
#include "graphics/2d.h"
#include "osapi/osregistry.h"
#include "osapi/osapi.h"
#include "cfile/cfile.h"
#include "object/object.h"
#include "localization/localize.h"
#include "mission/missionbriefcommon.h"
#include "ship/ship.h"
#include "species_defs/species_defs.h"
#include "ai/ai.h"
#include "weapon/weapon.h"
#include "stats/medals.h"
#include "iff_defs/iff_defs.h"
#include "starfield/starfield.h"
#include "nebula/neb.h"
#include "physics/physics.h"

#include "fredrender.h"

#include "mainwindow.h"

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
#ifdef SCP_UNIX
    setenv("force_s3tc_enable", "true", 1);
#endif
    //outwnd_init();
    QApplication app(argc, argv);
    QSplashScreen splash(QPixmap(":/images/splash.png"));
    splash.show();
    app.processEvents();

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

    std::vector<std::pair<std::function<void(void)>, QString>> initializers = {
        {std::bind(lcl_init, FS2_OPEN_DEFAULT_LANGUAGE), app.tr("Initialization locale")},
        {std::bind(gr_init, GR_OPENGL, 640, 480, 32), app.tr("Initializating graphics")},
        {[](){ SDL_HideWindow(os_get_window()); }, app.tr("Hiding FreeSpace root window")},
        {key_init, app.tr("Initializing keyboard")},
        {mouse_init, app.tr("Initializing mouse")},
        {iff_init, app.tr("Initializing IFF")},
        {obj_init, app.tr("Initializing objects")},
        {species_init, app.tr("Initializing species")},
        {mission_brief_common_init, app.tr("Initializing briefings")},
        {ai_init, app.tr("Initializing AI")},
        {ai_profiles_init, app.tr("Initializing AI profiles")},
        {armor_init, app.tr("Initializing armors")},
        {weapon_init, app.tr("Initializing weaponry")},
        {parse_medal_tbl, app.tr("Initializing medals")},
        {ship_init, app.tr("Initializing ships")},
        {neb2_init, app.tr("Initializing nebulas")},
        {stars_init, app.tr("Initializing stars")},
        {std::bind(stars_pre_level_init, true), app.tr("Pre-intializing stars levels")},
        {stars_post_level_init, app.tr("Initializing stars post levels")},
        {[]{
            gr_reset_clip();
            g3_start_frame(0);
            g3_set_view_matrix(&eye_pos, &eye_orient, 0.5f);
        }, app.tr("Setting view")}
    };

    for (const auto &initializer : initializers) {
        const auto &init_function = initializer.first;
        const auto &message = initializer.second;
        splash.showMessage(message, Qt::AlignHCenter | Qt::AlignBottom, Qt::white);
        app.processEvents();
        init_function();
    }

    fso::fred::MainWindow mw;
    splash.showMessage(qApp->tr("Switching rendering window"), Qt::AlignHCenter | Qt::AlignBottom, Qt::white);
    app.processEvents();
    os_set_window_from_hwnd(mw.effectiveWinId());
    mw.show();
    splash.finish(&mw);


    return app.exec();
}
