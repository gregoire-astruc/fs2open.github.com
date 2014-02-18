
SET(MINIZIP_SOURCES
	ioapi.c
	unzip.c
	zip.c
	crypt.h
	ioapi.h
	unzip.h
	zip.h
)

ADD_LIBRARY(minizip ${MINIZIP_SOURCES})
TARGET_LINK_LIBRARIES(minizip zlib)
ADD_DEFINITIONS(-DZLIB_INTERNAL)
		
set_target_properties(minizip
	PROPERTIES
		COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS
		DEFINE_SYMBOL ZLIB_DLL # This will only be defined if we are building a shared library
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
		FOLDER "3rdparty"
)
