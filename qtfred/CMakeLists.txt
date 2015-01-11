# largely inspired from http://doc.qt.io/qt-5/cmake-manual.html
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

if(WIN32)
    foreach( Qt_library ${QT_LIBRARIES} )
        message("${Qt_library}")
    endforeach( Qt_library ${QT_LIBRARIES} )
endif(WIN32)

qt5_wrap_ui(qtfred_UIS ui/mainwindow.ui)
qt5_wrap_cpp(qtfred_MOC include/renderwidget.h include/mainwindow.h)
qt5_add_resources(qtfred_QRC resources/resources.qrc)

set(QTFRED_SRC
    src/main.cpp
    src/fredrender.cpp
    src/fredstubs.cpp
    src/object.cpp
    src/renderwidget.cpp
    src/mainwindow.cpp
)

set(QTFRED_HEADERS
    include/editor.h
    include/fredrender.h
    include/mainwindow.h
    include/object.h
    include/renderwidget.h
    include/wing.h
)

add_executable(qtfred ${EXE_GUI_TYPE}
    ${QTFRED_SRC}
    ${QTFRED_HEADERS}
    ${qtfred_MOC}
    ${qtfred_UIS}
    ${qtfred_QRC}
)
set_target_properties(qtfred PROPERTIES OUTPUT_NAME "qtfred_${FSO_BINARY_SUFFIX}")

add_definitions(-DFRED)
if(UNIX)
  add_definitions(-DSCP_UNIX -DLUA_USE_LINUX)
endif(UNIX)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CODE_HEADERS}
  ${SDL_HEADERS}
  ${GENERATED_SOURCE_DIR}
  ${COMPILER_INCLUDES}
  ${PLATFORM_INCLUDES}
)

target_link_libraries(qtfred
    code
    Qt5::Widgets Qt5::OpenGL
    ${PLATFORM_LBRARIES} ${COMPILER_LIBRARIES})

include(CreateLaunchers)
create_target_launcher(qtfred
    WORKING_DIRECTORY ${FSO_FREESPACE_PATH}/data
    ARGS ${FSO_RUN_ARGUMENTS})

INSTALL(
    TARGETS qtfred
    RUNTIME DESTINATION ${BINARY_DESTINATION}
    BUNDLE DESTINATION ${BINARY_DESTINATION}
    CONFIGURATIONS Debug Release
)
COPY_FILES_TO_TARGET(qtfred)

if (WIN32)
    get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_BINS OUTPUT_VARIABLE QT_INSTALL_BINS OUTPUT_STRIP_TRAILING_WHITESPACE)

    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/run_$<CONFIG>.cmake"
    CONTENT "
include(BundleUtilities)
fixup_bundle(\"$<TARGET_FILE:qtfred>\" \"\" \"${QT_INSTALL_BINS}\")
    ")

    add_custom_command(TARGET qtfred
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/run_$<CONFIG>.cmake"
    VERBATIM)
endif(WIN32)
