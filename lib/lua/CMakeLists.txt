
IF(FSO_USE_LUAJIT)
	MESSAGE(STATUS "Using luajit as lua interpreter")

	INCLUDE(ExternalProject)
	INCLUDE(util)
	
	IF(FSO_DEVELOPMENT_MODE)
		SET(CONFIG Debug)
	ELSE(FSO_DEVELOPMENT_MODE)
		SET(CONFIG Release)
	ENDIF(FSO_DEVELOPMENT_MODE)

	ExternalProject_Add(luajit
		PREFIX luajit
		# Downloading
		GIT_REPOSITORY https://github.com/LuaDist/luajit.git
		GIT_TAG 2.0.3
		
		INSTALL_DIR
		
		# Building
		CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DCMAKE_BUILD_TYPE:STRING=${CONFIG}
		
		BUILD_COMMAND "${CMAKE_COMMAND}" --build <BINARY_DIR> --config ${CONFIG}
		
		INSTALL_COMMAND "${CMAKE_COMMAND}" --build <BINARY_DIR> --config ${CONFIG} --target install
	)

	set_target_properties(luajit
		PROPERTIES
			FOLDER "3rdparty"
	)

	ExternalProject_Get_Property(luajit install_dir)

	add_library(luajit_lib SHARED IMPORTED GLOBAL)
	ADD_DEPENDENCIES(luajit_lib luajit)

	IF(WIN32)
		set_target_properties(luajit_lib
			PROPERTIES
				IMPORTED_LOCATION ${install_dir}/bin/lua.dll
				IMPORTED_IMPLIB ${install_dir}/lib/lua.lib
		)
		
		INSTALL(FILES ${install_dir}/bin/lua.dll
				DESTINATION ${BINARY_DESTINATION}
		)
			
		SET(TARGET_COPY_FILES ${TARGET_COPY_FILES} "${install_dir}/bin/lua.dll" CACHE INTERNAL "")
	ELSE(WIN32)
		set_target_properties(luajit_lib
			PROPERTIES
				IMPORTED_LOCATION ${install_dir}/lib/liblua.a
		)
	ENDIF(WIN32)
	
	set_target_properties(luajit_lib
		PROPERTIES
			FOLDER "3rdparty"
			INTERFACE_COMPILE_DEFINITIONS LUA_BUILD_AS_DLL
			INTERFACE_INCLUDE_DIRECTORIES "${install_dir}/include"
	)

	FILE(MAKE_DIRECTORY "${install_dir}/include")

	SET(LUA_LIBS luajit_lib CACHE INTERNAL "LuaJIT library")
ELSE(FSO_USE_LUAJIT)
	IF(NOT FSO_BUILD_INCLUDED_LIBS)
		FIND_PACKAGE(Lua51 QUIET)
	ENDIF(NOT FSO_BUILD_INCLUDED_LIBS)

	IF (FSO_BUILD_INCLUDED_LIBS OR NOT LUA51_FOUND)
		MESSAGE(STATUS "Building lua from source")
		
		SET(LUA51_SOURCES
			lapi.c
			lauxlib.c
			lbaselib.c
			lcode.c
			ldblib.c
			ldebug.c
			ldo.c
			ldump.c
			lfunc.c
			lgc.c
			linit.c
			liolib.c
			llex.c
			lmathlib.c
			lmem.c
			loadlib.c
			lobject.c
			lopcodes.c
			loslib.c
			lparser.c
			lstate.c
			lstring.c
			lstrlib.c
			ltable.c
			ltablib.c
			ltm.c
			lua.c
			lundump.c
			lvm.c
			lzio.c
			print.c
			lapi.h
			lauxlib.h
			lcode.h
			ldebug.h
			ldo.h
			lfunc.h
			lgc.h
			llex.h
			llimits.h
			lmem.h
			lobject.h
			lopcodes.h
			lparser.h
			lstate.h
			lstring.h
			ltable.h
			ltm.h
			lua.h
			luaconf.h
			lualib.h
			lundump.h
			lvm.h
			lzio.h
			)

		ADD_LIBRARY(lua51 ${LUA51_SOURCES})
		
		INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
		
		set_target_properties(lua51
			PROPERTIES
				COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS
				COMPILE_DEFINITIONS_DEBUG LUA_USE_APICHECK
				DEFINE_SYMBOL LUA_BUILD_AS_DLL # This will only be defined if we are building a shared library
				INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
		)

		IF(BUILD_SHARED_LIBS)
			set_target_properties(lua51
				PROPERTIES
					INTERFACE_COMPILE_DEFINITIONS LUA_BUILD_AS_DLL
			)
		ENDIF(BUILD_SHARED_LIBS)

		set_target_properties(lua51
			PROPERTIES
				FOLDER "3rdparty"
		)
		
		IF(MSVC)
			set_target_properties(lua51
				PROPERTIES
					STATIC_LIBRARY_FLAGS "/LTCG"
			)
		ENDIF(MSVC)
		
		SET(LUA_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "Lua 5.1 include directory")
		
		SET(LUA_LIBS lua51 CACHE INTERNAL "Lua 5.1 library")

		IF(FSO_COPY_EXE AND BUILD_SHARED_LIBS)
			INSTALL(
				TARGETS lua51
				RUNTIME DESTINATION ${BINARY_DESTINATION}
				LIBRARY DESTINATION ${BINARY_DESTINATION}
				CONFIGURATIONS Debug Release
				)
		ENDIF(FSO_COPY_EXE AND BUILD_SHARED_LIBS)
	ELSE(FSO_BUILD_INCLUDED_LIBS OR NOT LUA51_FOUND)
		INCLUDE(util)

		ADD_IMPORTED_LIB(lua "${LUA_INCLUDE_DIR}" "${LUA_LIBRARIES}" SHARED)

		SET(LUA_LIBS lua CACHE INTERNAL "lua library" FORCE)
	ENDIF(FSO_BUILD_INCLUDED_LIBS OR NOT LUA51_FOUND)

ENDIF(FSO_USE_LUAJIT)
