
INCLUDE(DownloadProject)

ADD_SUBDIRECTORY(FreeType)

include(ExternalProject)

IF(FSO_DEVELOPMENT_MODE)
	SET(CONFIG Debug)
ELSE(FSO_DEVELOPMENT_MODE)
	SET(CONFIG Release)
ENDIF(FSO_DEVELOPMENT_MODE)

ExternalProject_Add(freetype-gl
	PREFIX freetype-gl
	# Downloading
	GIT_REPOSITORY https://github.com/asarium/freetype-gl.git
	GIT_TAG restructuring
	
	#SOURCE_DIR H:/Code/Github/freetype-gl
	CMAKE_CACHE_ARGS -DUSE_GLEW:BOOL=OFF -Dfreetype-gl_BUILD_DEMOS:BOOL=OFF
					-DMSVC_STATIC_RUNTIME:BOOL=ON
					-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DCMAKE_BUILD_TYPE:STRING=${CONFIG}
	
	BUILD_COMMAND "${CMAKE_COMMAND}" --build <BINARY_DIR> --config ${CONFIG}
	
	INSTALL_COMMAND "${CMAKE_COMMAND}" --build <BINARY_DIR> --config ${CONFIG} --target install
)

set_target_properties(freetype-gl
	PROPERTIES
		FOLDER "3rdparty"
)

ExternalProject_Get_Property(freetype-gl install_dir)

add_library(freetype-gl_lib STATIC IMPORTED GLOBAL)
ADD_DEPENDENCIES(freetype-gl_lib freetype-gl)

IF(WIN32)
	set_target_properties(freetype-gl_lib
		PROPERTIES
			IMPORTED_LOCATION ${install_dir}/lib/freetype-gl.lib
	)
ELSE(WIN32)
	set_target_properties(freetype-gl_lib
		PROPERTIES
			IMPORTED_LOCATION ${install_dir}/lib/libfreetype-gl.so
	)
ENDIF(WIN32)

set_target_properties(freetype-gl_lib
	PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${install_dir}/include;${FREETYPE_INCLUDE_DIRS}"
		IMPORTED_LINK_INTERFACE_LIBRARIES ${FREETYPE_LIBRARIES}
)
# CMake complains about non-existent firectories otherwise
FILE(MAKE_DIRECTORY ${install_dir}/include)

SET(FREETYPE_GL_LIBRARY freetype-gl_lib CACHE INTERNAL "OpenAL library")
