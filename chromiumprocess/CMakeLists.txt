

SET(EXTENSION_HEADER_FILE "${GENERATED_SOURCE_DIR}/chromiumprocess/fsoExtension.js.h")
# Generate the java script extension header
FILE(MAKE_DIRECTORY "${GENERATED_SOURCE_DIR}/chromiumprocess")

ADD_CUSTOM_COMMAND(
	OUTPUT ${EXTENSION_HEADER_FILE}
	COMMAND embedfile -text "${CMAKE_CURRENT_SOURCE_DIR}/resources/fsoExtension.js" "${EXTENSION_HEADER_FILE}" "FSOExtensionCode"
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/resources/fsoExtension.js"
	COMMENT "Generating string file for ${CMAKE_CURRENT_SOURCE_DIR}/resources/fsoExtension.js"
)

set(SOURCES
	main.h
	main.cpp
	Application.h
	Application.cpp
	${CODE_HEADERS}/chromium/jsapi/jsapi.h
	${EXTENSION_HEADER_FILE}
	resources/fsoExtension.js
)

add_executable(chromiumprocess ${EXE_GUI_TYPE} ${SOURCES})

source_group(Resources FILES resources/fsoExtension.js)

SET_TARGET_PROPERTIES(chromiumprocess PROPERTIES OUTPUT_NAME "chromium/chromiumprocess_${FSO_BINARY_SUFFIX}")
SET_TARGET_PROPERTIES(chromiumprocess PROPERTIES DEBUG_POSTFIX "-DEBUG")

IF(MSVC)
	# target_link_libraries(chromiumprocess cef_dll_wrapper)
	
	set_target_properties(chromiumprocess PROPERTIES
		COMPILE_DEFINITIONS_DEBUG "_DEBUG"
	)
ELSEIF(UNIX)
	target_link_libraries(chromiumprocess pthread)
ENDIF(MSVC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CODE_HEADERS}/chromium/jsapi)
include_directories(${GENERATED_SOURCE_DIR}/chromiumprocess)

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_definitions(-DBUILDING_CHROMIUMPROCESS)

install(
	TARGETS chromiumprocess
	RUNTIME DESTINATION ${BINARY_DESTINATION}
	BUNDLE DESTINATION ${BINARY_DESTINATION}
	CONFIGURATIONS Debug Release
)

if(CEF_PATH)
	add_custom_command(
	    TARGET chromiumprocess
	    PRE_LINK
	    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CEF_PATH}/Resources $<TARGET_FILE_DIR:chromiumprocess>
	    COMMENT "Copying CEF resources..."
	)
endif(CEF_PATH)

INCLUDE(util)
INCLUDE(cef)
CEF_TARGET(chromiumprocess)
COPY_FILES_TO_TARGET(chromiumprocess)
