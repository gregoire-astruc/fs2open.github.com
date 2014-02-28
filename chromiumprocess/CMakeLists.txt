
set(SOURCES
	main.h
	main.cpp
	Application.h
	Application.cpp
	${file_root_chromium_jsapi}
)

add_executable(chromiumprocess ${EXE_GUI_TYPE} ${SOURCES})
target_link_libraries(chromiumprocess cef_cpp_wrapper)

IF(MSVC)
	set_target_properties(chromiumprocess PROPERTIES
		COMPILE_DEFINITIONS_DEBUG "_DEBUG"
	)
ENDIF(MSVC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CODE_HEADERS}/chromium/jsapi)

IF(MSVC)
	IF(MSVC_USE_RUNTIME_DLL)
		SET(Boost_USE_STATIC_RUNTIME FALSE)
	ELSE(MSVC_USE_RUNTIME_DLL)
		SET(Boost_USE_STATIC_RUNTIME TRUE)
	ENDIF(MSVC_USE_RUNTIME_DLL)
ENDIF(MSVC)

SET(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS system)

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(chromiumprocess ${Boost_LIBRARIES})

add_definitions(-DBUILDING_CHROMIUMPROCESS)
add_definitions(-DBOOST_ALL_NO_LIB)

install(
	TARGETS chromiumprocess
	RUNTIME DESTINATION ${BINARY_DESTINATION}
	BUNDLE DESTINATION ${BINARY_DESTINATION}
	CONFIGURATIONS Debug Release
)

INCLUDE(util)
COPY_FILES_TO_TARGET(chromiumprocess)
