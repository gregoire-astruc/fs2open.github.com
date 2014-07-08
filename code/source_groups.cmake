# top-level files
set (file_root
)

# AI files
set (file_root_ai
	ai/ai.cpp
	ai/ai_profiles.cpp
	ai/aibig.cpp
	ai/aicode.cpp
	ai/aigoals.cpp
	ai/aiturret.cpp
)

# Anim files
set (file_root_anim
	anim/animplay.cpp
	anim/packunpack.cpp
)

# Asteroid files
set (file_root_asteroid
	asteroid/asteroid.cpp
)

# Autopilot files
set (file_root_autopilot
	autopilot/autopilot.cpp
)

# Bmpman files
set (file_root_bmpman
	bmpman/bmpman.cpp
)

# Camera files
set (file_root_camera
	camera/camera.cpp
)

# CFile files
set (file_root_cfile
	cfile/cfile.cpp
	cfile/VPFileSystem.cpp
	cfile/VPFileSystemEntry.cpp
	cfile/ZipFileSystem.cpp
	cfile/ZipFileSystemEntry.cpp
)

# Chromium files
set(file_root_chromium
	chromium/chromium.cpp
	chromium/ApplicationImpl.cpp
	chromium/Browser.cpp
	chromium/ChromiumStateLogic.cpp
	chromium/ClientImpl.cpp
)

# jsapi Files
set(file_root_chromium_jsapi
	chromium/jsapi/jsapi.cpp
)

# Cmdline files
set (file_root_cmdline
	cmdline/cmdline.cpp
)

# CMeasure files
set (file_root_cmeasure
	cmeasure/cmeasure.cpp
)

# ControlConfig files
set (file_root_controlconfig
	controlconfig/controlsconfig.cpp
	controlconfig/controlsconfigcommon.cpp
)

# Cutscene files
set (file_root_cutscene
	cutscene/cutscenes.cpp
	cutscene/decoder16.cpp
	cutscene/decoder8.cpp
	cutscene/movie.cpp
	cutscene/mve_audio.cpp
	cutscene/mvelib.cpp
	cutscene/mveplayer.cpp
	cutscene/oggplayer.cpp
)

# ddsutils files
set (file_root_ddsutils
	ddsutils/ddsutils.cpp
)

# Debris files
set (file_root_debris
	debris/debris.cpp
)

# DebugConsole files
set (file_root_debugconsole
	debugconsole/console.cpp
	debugconsole/timerbar.cpp
)

SET(file_root_def_files
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/ai_profiles.tbl
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/autopilot.tbl
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/blur-f.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/brightpass-f.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/controlconfigdefaults.tbl
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/fonts.tbl
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/fxaa-f.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/fxaa-v.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/fxaapre-f.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/game_settings.tbl
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/iff_defs.tbl
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/ls-f.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/main-f.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/main-v.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/objecttypes.tbl
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/particle-f.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/particle-v.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/post-f.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/post-v.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/post_processing.tbl
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/species_defs.tbl
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/video-f.sdr
	${CMAKE_CURRENT_SOURCE_DIR}/def_files/video-v.sdr)

# DirectX files
set (file_root_directx
)

# ExceptionHandler files
set (file_root_exceptionhandler
	exceptionhandler/exceptionhandler.cpp
)

# ExternalDLL files
set (file_root_externaldll
	external_dll/trackirglobal.cpp
)

# Fireball files
set (file_root_fireball
	fireball/fireballs.cpp
	fireball/warpineffect.cpp
)

# GameHelp files
set (file_root_gamehelp
	gamehelp/contexthelp.cpp
	gamehelp/gameplayhelp.cpp
)

# GameHelp -> fs2netd files
set (file_root_gamehelp_fs2netd
	fs2netd/fs2netd_client.cpp
	fs2netd/tcp_client.cpp
	fs2netd/tcp_socket.cpp
)

# GameSequence files
set (file_root_gamesequence
	gamesequence/gamesequence.cpp
)

# GameSnd files
set (file_root_gamesnd
	gamesnd/eventmusic.cpp
	gamesnd/gamesnd.cpp
)

set(file_root_generated
)

# GlobalIncs files
set (file_root_globalincs
	globalincs/alphacolors.cpp
	globalincs/def_files.cpp
	globalincs/fsmemory.cpp
	globalincs/profiling.cpp
	globalincs/safe_strings.cpp
	globalincs/safe_strings_test.cpp
	globalincs/systemvars.cpp
	globalincs/util.cpp
	globalincs/version.cpp
)

IF (WIN32)
	set (file_root_globalincs
		${file_root_globalincs}
		globalincs/mspdb_callstack.cpp
		globalincs/windebug.cpp
	)
ENDIF(WIN32)

# Graphics files
set (file_root_graphics
	graphics/2d.cpp
	graphics/grbatch.cpp
)

# Graphics -> OpenGLGr files
set (file_root_graphics_openglgr
)

# Graphics -> OpenGLGr -> OpenGL CPPs files
set (file_root_graphics_openglgr_opengl_cpps
	graphics/gropengl.cpp
	graphics/gropenglbmpman.cpp
	graphics/gropengldraw.cpp
	graphics/gropenglextension.cpp
	graphics/gropengllight.cpp
	graphics/gropenglpostprocessing.cpp
	graphics/gropenglshader.cpp
	graphics/gropenglstate.cpp
	graphics/gropengltexture.cpp
	graphics/gropengltnl.cpp
)

# Graphics -> OpenGLGr -> OpenGL Headers files
set (file_root_graphics_openglgr_opengl_headers
)

# Graphics -> SoftwareGr files
set (file_root_graphics_softwaregr
	graphics/font.cpp
	graphics/generic.cpp
	graphics/grstub.cpp
)

# Hud files
set (file_root_hud
	hud/hud.cpp
	hud/hudartillery.cpp
	hud/hudbrackets.cpp
	hud/hudconfig.cpp
	hud/hudescort.cpp
	hud/hudets.cpp
	hud/hudlock.cpp
	hud/hudmessage.cpp
	hud/hudnavigation.cpp
	hud/hudobserver.cpp
	hud/hudparse.cpp
	hud/hudreticle.cpp
	hud/hudshield.cpp
	hud/hudsquadmsg.cpp
	hud/hudtarget.cpp
	hud/hudtargetbox.cpp
	hud/hudwingmanstatus.cpp
)

# iff_defs files
set (file_root_iff_defs
	iff_defs/iff_defs.cpp
)

# InetFile files
set (file_root_inetfile
	inetfile/cftp.cpp
	inetfile/chttpget.cpp
	inetfile/inetgetfile.cpp
)

# Io files
set (file_root_io
	io/cursor.cpp
	io/key.cpp
	io/keycontrol.cpp
	io/mouse.cpp
	io/timer.cpp
	io/joy-sdl.cpp
	io/joy_ff-sdl.cpp
)

# jpgutils files
set (file_root_jpgutils
	jpgutils/jpgutils.cpp
)

# JumpNode files
set (file_root_jumpnode
	jumpnode/jumpnode.cpp
)

# Lab files
set (file_root_lab
	lab/lab.cpp
	lab/wmcgui.cpp
)

# Lighting files
set (file_root_lighting
	lighting/lighting.cpp
)

# Localization files
set (file_root_localization
	localization/fhash.cpp
	localization/localize.cpp
)

# Mainloop files
set(file_root_mainloop
	mainloop/mainloop.cpp
)

# Math files
set (file_root_math
	math/fix.cpp
	math/floating.cpp
	math/fvi.cpp
	math/spline.cpp
	math/staticrand.cpp
	math/vecmat.cpp
)

# MenuUI files
set (file_root_menuui
	menuui/barracks.cpp
	menuui/credits.cpp
	menuui/fishtank.cpp
	menuui/mainhallmenu.cpp
	menuui/mainhalltemp.cpp
	menuui/optionsmenu.cpp
	menuui/optionsmenumulti.cpp
	menuui/playermenu.cpp
	menuui/readyroom.cpp
	menuui/snazzyui.cpp
	menuui/techmenu.cpp
	menuui/trainingmenu.cpp
)

# Mission files
set (file_root_mission
	mission/missionbriefcommon.cpp
	mission/missioncampaign.cpp
	mission/missiongoals.cpp
	mission/missiongrid.cpp
	mission/missionhotkey.cpp
	mission/missionload.cpp
	mission/missionlog.cpp
	mission/missionmessage.cpp
	mission/missionparse.cpp
	mission/missiontraining.cpp
)

# MissionUI files
set (file_root_missionui
	missionui/chatbox.cpp
	missionui/fictionviewer.cpp
	missionui/missionbrief.cpp
	missionui/missioncmdbrief.cpp
	missionui/missiondebrief.cpp
	missionui/missionloopbrief.cpp
	missionui/missionpause.cpp
	missionui/missionrecommend.cpp
	missionui/missionscreencommon.cpp
	missionui/missionshipchoice.cpp
	missionui/missionstats.cpp
	missionui/missionweaponchoice.cpp
	missionui/redalert.cpp
)

# mod_table files
set (file_root_mod_table
	mod_table/mod_table.cpp
)

# Model files
set (file_root_model
	model/modelanim.cpp
	model/modelcollide.cpp
	model/modelinterp.cpp
	model/modeloctant.cpp
	model/modelread.cpp
)

# Nebula files
set (file_root_nebula
	nebula/neb.cpp
	nebula/neblightning.cpp
)

# Network files
set (file_root_network
	network/chat_api.cpp
	network/multi.cpp
	network/multi_campaign.cpp
	network/multi_data.cpp
	network/multi_dogfight.cpp
	network/multi_endgame.cpp
	network/multi_ingame.cpp
	network/multi_kick.cpp
	network/multi_log.cpp
	network/multi_obj.cpp
	network/multi_observer.cpp
	network/multi_options.cpp
	network/multi_pause.cpp
	network/multi_pinfo.cpp
	network/multi_ping.cpp
	network/multi_pmsg.cpp
	network/multi_pxo.cpp
	network/multi_rate.cpp
	network/multi_respawn.cpp
	network/multi_sexp.cpp
	network/multi_team.cpp
	network/multi_update.cpp
	network/multi_voice.cpp
	network/multi_xfer.cpp
	network/multilag.cpp
	network/multimsgs.cpp
	network/multiteamselect.cpp
	network/multiui.cpp
	network/multiutil.cpp
	network/psnet2.cpp
	network/stand_gui.cpp
)

IF(WIN32)
set (file_root_network
	${file_root_network}
	network/stand_gui.cpp
)
ELSE(WIN32)
set (file_root_network
	${file_root_network}
	network/stand_gui-unix.cpp
)
ENDIF(WIN32)

# Object files
set (file_root_object
	object/collidedebrisship.cpp
	object/collidedebrisweapon.cpp
	object/collideshipship.cpp
	object/collideshipweapon.cpp
	object/collideweaponweapon.cpp
	object/deadobjectdock.cpp
	object/objcollide.cpp
	object/object.cpp
	object/objectdock.cpp
	object/objectshield.cpp
	object/objectsnd.cpp
	object/objectsort.cpp
	object/parseobjectdock.cpp
	object/waypoint.cpp
)

# Observer files
set (file_root_observer
	observer/observer.cpp
)

# OsApi files
set (file_root_osapi
	osapi/osapi.cpp
)

IF(WIN32)
set (file_root_osapi
	${file_root_osapi}
	osapi/osregistry.cpp
	osapi/outwnd.cpp
)
ELSEIF(UNIX)
set (file_root_osapi
	${file_root_osapi}
	osapi/osregistry_unix.cpp
	osapi/outwnd_unix.cpp
)
ENDIF(WIN32)

# Palman files
set (file_root_palman
	palman/palman.cpp
)

# Parse files
set (file_root_parse
	parse/encrypt.cpp
	parse/generic_log.cpp
	parse/lua.cpp
	parse/parselo.cpp
	parse/scripting.cpp
	parse/sexp.cpp
)

set(file_root_parse_lua
	parse/lua/util.cpp
)

# Particle files
set (file_root_particle
	particle/particle.cpp
)

# PcxUtils files
set (file_root_pcxutils
	pcxutils/pcxutils.cpp
)

# Physics files
set (file_root_physics
	physics/physics.cpp
)

# PilotFile files
set (file_root_pilotfile
	pilotfile/csg.cpp
	pilotfile/csg_convert.cpp
	pilotfile/pilotfile.cpp
	pilotfile/pilotfile_convert.cpp
	pilotfile/plr.cpp
	pilotfile/plr_convert.cpp
)

# Playerman files
set (file_root_playerman
	playerman/managepilot.cpp
	playerman/playercontrol.cpp
)

# pngutils files
set (file_root_pngutils
	pngutils/pngutils.cpp
)

# Popup files
set (file_root_popup
	popup/popup.cpp
	popup/popupdead.cpp
)

# Radar files
set (file_root_radar
	radar/radar.cpp
	radar/radardradis.cpp
	radar/radarorb.cpp
	radar/radarsetup.cpp
)

# Render files
set (file_root_render
	render/3dclipper.cpp
	render/3ddraw.cpp
	render/3dlaser.cpp
	render/3dmath.cpp
	render/3dsetup.cpp
)

# Ship files
set (file_root_ship
	ship/afterburner.cpp
	ship/awacs.cpp
	ship/shield.cpp
	ship/ship.cpp
	ship/shipcontrails.cpp
	ship/shipfx.cpp
	ship/shiphit.cpp
)

# Sound files
set (file_root_sound
	sound/acm.cpp
	sound/audiostr.cpp
	sound/ds.cpp
	sound/ds3d.cpp
	sound/dscap.cpp
	sound/fsspeech.cpp
	sound/openal.cpp
	sound/phrases.xml
	sound/rtvoice.cpp
	sound/sound.cpp
	sound/speech.cpp
	sound/voicerec.cpp
)

# Sound -> ogg files
set (file_root_sound_ogg
	sound/ogg/ogg.cpp
)

# Species_Defs files
set (file_root_species_defs
	species_defs/species_defs.cpp
)

# Starfield files
set (file_root_starfield
	starfield/nebula.cpp
	starfield/starfield.cpp
	starfield/supernova.cpp
)

set(file_root_statelogic
	statelogic/statelogic.cpp
)

# Stats files
set (file_root_stats
	stats/medals.cpp
	stats/scoring.cpp
	stats/stats.cpp
)

# TgaUtils files
set (file_root_tgautils
	tgautils/tgautils.cpp
)

# Ui files
set (file_root_ui
	ui/button.cpp
	ui/checkbox.cpp
	ui/gadget.cpp
	ui/icon.cpp
	ui/inputbox.cpp
	ui/keytrap.cpp
	ui/listbox.cpp
	ui/radio.cpp
	ui/scroll.cpp
	ui/slider.cpp
	ui/slider2.cpp
	ui/uidraw.cpp
	ui/uimouse.cpp
	ui/window.cpp
)

# Weapon files
set (file_root_weapon
	weapon/beam.cpp
	weapon/corkscrew.cpp
	weapon/emp.cpp
	weapon/flak.cpp
	weapon/muzzleflash.cpp
	weapon/shockwave.cpp
	weapon/swarm.cpp
	weapon/trails.cpp
	weapon/weapons.cpp
)

# Windows stubs files
set(file_root_windows_stubs
)

IF(UNIX)
	SET(file_root_windows_stubs
		${file_root_windows_stubs}
		windows_stub/stubs.cpp
	)
ENDIF(UNIX)

# the source groups
source_group(""                                   FILES ${file_root})
source_group("AI"                                 FILES ${file_root_ai})
source_group("Anim"                               FILES ${file_root_anim})
source_group("Asteroid"                           FILES ${file_root_asteroid})
source_group("Autopilot"                          FILES ${file_root_autopilot})
source_group("Bmpman"                             FILES ${file_root_bmpman})
source_group("Camera"                             FILES ${file_root_camera})
source_group("CFile"                              FILES ${file_root_cfile})
source_group("Cmdline"                            FILES ${file_root_cmdline})
source_group("CMeasure"                           FILES ${file_root_cmeasure})
source_group("Chromium"                           FILES ${file_root_chromium})
source_group("Chromium\\JSAPI"                    FILES ${file_root_chromium_jsapi})
source_group("ControlConfig"                      FILES ${file_root_controlconfig})
source_group("Cutscene"                           FILES ${file_root_cutscene})
source_group("ddsutils"                           FILES ${file_root_ddsutils})
source_group("Debris"                             FILES ${file_root_debris})
source_group("DebugConsole"                       FILES ${file_root_debugconsole})
SOURCE_GROUP("Default files"                      FILES ${file_root_def_files})

IF(WIN32)
	source_group("DirectX"                        FILES ${file_root_directx})
ENDIF(WIN32)

source_group("ExceptionHandler"                   FILES ${file_root_exceptionhandler})
source_group("ExternalDLL"                        FILES ${file_root_externaldll})
source_group("Fireball"                           FILES ${file_root_fireball})
source_group("GameHelp"                           FILES ${file_root_gamehelp})
source_group("GameHelp\\fs2netd"                  FILES ${file_root_gamehelp_fs2netd})
source_group("GameSequence"                       FILES ${file_root_gamesequence})
source_group("GameSnd"                            FILES ${file_root_gamesnd})
source_group("Generated Files"                    FILES ${file_root_generated})
source_group("GlobalIncs"                         FILES ${file_root_globalincs})
source_group("Graphics"                           FILES ${file_root_graphics})
source_group("Graphics\\OpenGLGr"                 FILES ${file_root_graphics_openglgr})
source_group("Graphics\\OpenGLGr\\OpenGL CPPs"    FILES ${file_root_graphics_openglgr_opengl_cpps})
source_group("Graphics\\OpenGLGr\\OpenGL Headers" FILES ${file_root_graphics_openglgr_opengl_headers})
source_group("Graphics\\SoftwareGr"               FILES ${file_root_graphics_softwaregr})
source_group("Hud"                                FILES ${file_root_hud})
source_group("iff_defs"                           FILES ${file_root_iff_defs})
source_group("InetFile"                           FILES ${file_root_inetfile})
source_group("Io"                                 FILES ${file_root_io})
source_group("jpgutils"                           FILES ${file_root_jpgutils})
source_group("JumpNode"                           FILES ${file_root_jumpnode})
source_group("Lab"                                FILES ${file_root_lab})
source_group("Lighting"                           FILES ${file_root_lighting})
source_group("Localization"                       FILES ${file_root_localization})
source_group("Mainloop"                               FILES ${file_root_mainloop})
source_group("Math"                               FILES ${file_root_math})
source_group("MenuUI"                             FILES ${file_root_menuui})
source_group("Mission"                            FILES ${file_root_mission})
source_group("MissionUI"                          FILES ${file_root_missionui})
source_group("mod_table"                          FILES ${file_root_mod_table})
source_group("Model"                              FILES ${file_root_model})
source_group("Nebula"                             FILES ${file_root_nebula})
source_group("Network"                            FILES ${file_root_network})
source_group("Object"                             FILES ${file_root_object})
source_group("Observer"                           FILES ${file_root_observer})
source_group("OsApi"                              FILES ${file_root_osapi})
source_group("Palman"                             FILES ${file_root_palman})
source_group("Parse"                              FILES ${file_root_parse})
source_group("Parse\\Lua"                         FILES ${file_root_parse_lua})
source_group("Particle"                           FILES ${file_root_particle})
source_group("PcxUtils"                           FILES ${file_root_pcxutils})
source_group("Physics"                            FILES ${file_root_physics})
source_group("PilotFile"                          FILES ${file_root_pilotfile})
source_group("Playerman"                          FILES ${file_root_playerman})
source_group("pngutils"                           FILES ${file_root_pngutils})
source_group("Popup"                              FILES ${file_root_popup})
source_group("Radar"                              FILES ${file_root_radar})
source_group("Render"                             FILES ${file_root_render})
source_group("Ship"                               FILES ${file_root_ship})
source_group("Sound"                              FILES ${file_root_sound})
source_group("Sound\\ogg"                         FILES ${file_root_sound_ogg})
source_group("Species_Defs"                       FILES ${file_root_species_defs})
source_group("Starfield"                          FILES ${file_root_starfield})
source_group("StateLogic"                         FILES ${file_root_statelogic})
source_group("Stats"                              FILES ${file_root_stats})
source_group("TgaUtils"                           FILES ${file_root_tgautils})
source_group("Ui"                                 FILES ${file_root_ui})
source_group("Weapon"                             FILES ${file_root_weapon})
source_group("Windows Stubs"                      FILES ${file_root_windows_stubs})

# append all files to the file_root
set (file_root
	${file_root}
	${file_root_ai}
	${file_root_anim}
	${file_root_asteroid}
	${file_root_autopilot}
	${file_root_bmpman}
	${file_root_camera}
	${file_root_cfile}
	${file_root_cmdline}
	${file_root_cmeasure}
	${file_root_chromium}
	${file_root_chromium_jsapi}
	${file_root_controlconfig}
	${file_root_cutscene}
	${file_root_ddsutils}
	${file_root_debris}
	${file_root_debugconsole}
	${file_root_def_files}
	${file_root_exceptionhandler}
	${file_root_externaldll}
	${file_root_fireball}
	${file_root_gamehelp}
	${file_root_gamehelp_fs2netd}
	${file_root_gamesequence}
	${file_root_gamesnd}
	${file_root_generated}
	${file_root_globalincs}
	${file_root_graphics}
	${file_root_graphics_openglgr}
	${file_root_graphics_openglgr_opengl_cpps}
	${file_root_graphics_openglgr_opengl_headers}
	${file_root_graphics_softwaregr}
	${file_root_hud}
	${file_root_iff_defs}
	${file_root_inetfile}
	${file_root_io}
	${file_root_jpgutils}
	${file_root_jumpnode}
	${file_root_lab}
	${file_root_lighting}
	${file_root_localization}
	${file_root_mainloop}
	${file_root_math}
	${file_root_menuui}
	${file_root_mission}
	${file_root_missionui}
	${file_root_mod_table}
	${file_root_model}
	${file_root_nebula}
	${file_root_network}
	${file_root_object}
	${file_root_observer}
	${file_root_osapi}
	${file_root_palman}
	${file_root_parse}
	${file_root_parse_lua}
	${file_root_particle}
	${file_root_pcxutils}
	${file_root_physics}
	${file_root_pilotfile}
	${file_root_playerman}
	${file_root_pngutils}
	${file_root_popup}
	${file_root_radar}
	${file_root_render}
	${file_root_ship}
	${file_root_sound}
	${file_root_sound_ogg}
	${file_root_species_defs}
	${file_root_starfield}
	${file_root_statelogic}
	${file_root_stats}
	${file_root_tgautils}
	${file_root_ui}
	${file_root_weapon}
	${file_root_windows_stubs}
)

IF(WIN32)
	set (file_root ${file_root} ${file_root_directx})
ENDIF(WIN32)
