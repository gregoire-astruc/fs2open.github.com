# largely inspired from http://doc.qt.io/qt-5/cmake-manual.html
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

add_executable(qtfred main.cpp
    fredrender.cpp
    fredstubs.cpp
    object.cpp
    renderwidget.cpp)
set_target_properties(qtfred PROPERTIES OUTPUT_NAME "qtfred_${FSO_BINARY_SUFFIX}")

add_definitions(-DFRED)

include_directories(
  ${CODE_HEADERS}
  ${SDL_HEADERS}
  ${GENERATED_SOURCE_DIR}
  ${COMPILER_INCLUDES}
  ${PLATFORM_INCLUDES}
)

target_link_libraries(qtfred
    code
    ${PLATFORM_LBRARIES}
    ${COMPILER_LIBRARIES}
    Qt5::Widgets Qt5::OpenGL)

if(UNIX)
  add_definitions(-DSCP_UNIX -DLUA_USE_LINUX)
endif(UNIX)
