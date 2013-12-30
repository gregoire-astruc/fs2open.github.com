
IF(UNIX)
	FIND_PACKAGE(FreeType REQUIRED)
ELSE(UNIX)
	# Use precompiled libs for windows
	
	add_library(freetype STATIC IMPORTED GLOBAL)
	set_target_properties(freetype
		PROPERTIES
			IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/freetype247MT.lib
	)
	SET(FREETYPE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "FreeType include directory")
	SET(FREETYPE_LIBRARIES freetype CACHE INTERNAL "FreeType library")
ENDIF(UNIX)

