

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

SET_TARGET_PROPERTIES(chromiumprocess PROPERTIES OUTPUT_NAME "chromiumprocess_${FSO_BINARY_SUFFIX}")
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

INCLUDE(cef)

if(CEF_PATH)
	add_custom_command(
	    TARGET chromiumprocess
	    PRE_LINK
	    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CEF_PATH}/Resources $<TARGET_FILE_DIR:chromiumprocess>/chromium
	    COMMENT "Copying CEF resources..."
	    VERBATIM
	)

	if(WIN32)
		set(LIB_EXT dll)
	elseif(UNIX)
		set(LIB_EXT so)
	endif(WIN32)

	file(GLOB CEF_LIBS ${CEF_PATH}/${CEF_BUILD_TYPE}/*.${LIB_EXT})
	list(LENGTH CEF_LIBS CEF_LIBS_SIZE)
	math(EXPR CEF_LIBS_END "${CEF_LIBS_SIZE} - 1")

	foreach(i RANGE 0 ${CEF_LIBS_END})
		list(GET CEF_LIBS ${i} LIB)
		math(EXPR num "${i} + 1")

		add_custom_command(
		    TARGET chromiumprocess
		    COMMAND ${CMAKE_COMMAND} -E copy ${LIB} $<TARGET_FILE_DIR:chromiumprocess>
		    COMMENT "[${num}/${CEF_LIBS_SIZE}] Copying CEF libraries..."
		    VERBATIM
		)
	endforeach(i)
endif(CEF_PATH)

INCLUDE(util)
CEF_TARGET(chromiumprocess)
COPY_FILES_TO_TARGET(chromiumprocess)
