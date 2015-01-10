# largely inspired from http://doc.qt.io/qt-5/cmake-manual.html
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)

qt5_wrap_ui(qtfred_UIS ui/mainwindow.ui)
qt5_wrap_cpp(qtfred_MOC include/renderwidget.h include/mainwindow.h)

add_executable(qtfred src/main.cpp
    src/fredrender.cpp
    src/fredstubs.cpp
    src/object.cpp
    src/renderwidget.cpp src/mainwindow.cpp
    ${qtfred_MOC}
    ${qtfred_UIS}
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
