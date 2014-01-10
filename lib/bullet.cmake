
include(ExternalProject)

IF(FSO_DEVELOPMENT_MODE)
	SET(CONFIG Debug)
ELSE(FSO_DEVELOPMENT_MODE)
	SET(CONFIG Release)
ENDIF(FSO_DEVELOPMENT_MODE)

IF(WIN32)
	SET(DOWNLOAD_URL https://bullet.googlecode.com/files/bullet-2.82-r2704.zip)
	SET(DOWNLOAD_SHA1 f4b3332ad074aef3f8c1b731c1b7b385d3386d31)
ELSE(WIN32)
	SET(DOWNLOAD_URL https://bullet.googlecode.com/files/bullet-2.82-r2704.tgz)
	SET(DOWNLOAD_SHA1 a0867257b9b18e9829bbeb4c6c5872a5b29d1d33)
ENDIF(WIN32)

ExternalProject_Add(bullet
	PREFIX bullet
	# Downloading
	URL ${DOWNLOAD_URL} # Full path or URL of source
	URL_HASH SHA1=${DOWNLOAD_SHA1}
	
	#SOURCE_DIR "H:/Code/C++/Libraries/bullet-2.82-r2704/"
		
	CMAKE_CACHE_ARGS -DBUILD_MINICL_OPENCL_DEMOS:BOOL=OFF -DBUILD_AMD_OPENCL_DEMOS:BOOL=OFF -DBUILD_INTEL_OPENCL_DEMOS:BOOL=OFF
						-DBUILD_NVIDIA_OPENCL_DEMOS:BOOL=OFF -DINSTALL_LIBS:BOOL=ON -DINSTALL_EXTRA_LIBS:BOOL=ON
						-DBUILD_CPU_DEMOS:BOOL=OFF -DBUILD_DEMOS:BOOL=OFF
						-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DCMAKE_BUILD_TYPE:STRING=${CONFIG}
						
	BUILD_COMMAND "${CMAKE_COMMAND}" --build <BINARY_DIR> --config ${CONFIG}
	
	INSTALL_COMMAND "${CMAKE_COMMAND}" --build <BINARY_DIR> --config ${CONFIG} --target install
)

set_target_properties(bullet
	PROPERTIES
		FOLDER "3rdparty"
)

ExternalProject_Get_Property(bullet install_dir)

set ( BULLET_INCLUDE_DIRS "${install_dir}/include/bullet" )
file(MAKE_DIRECTORY ${BULLET_INCLUDE_DIRS})

set ( BULLET_LIBRARIES    "LinearMath;BulletCollision;BulletDynamics;BulletSoftBody" )

set ( BULLET_LIBRARY_DIRS "${install_dir}/lib" )

set(lib_paths)

foreach(lib IN LISTS BULLET_LIBRARIES)
	set(lib_paths ${lib_paths} "${BULLET_LIBRARY_DIRS}/${CMAKE_STATIC_LIBRARY_PREFX}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
endforeach(lib)

include(util)
ADD_IMPORTED_LIB(bullet_lib "${BULLET_INCLUDE_DIRS}" "${lib_paths}" STATIC)
ADD_DEPENDENCIES(bullet_lib bullet)
	
set(BULLET_LIBS bullet_lib CACHE INTERNAL "")
