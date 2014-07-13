
if(NOT DEFINED CEF_BUILD_TYPE)
	if(WIN32)
		set(CEF_BUILD_TYPE "$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>")
	else(WIN32)
		# Default to Release
		set(CEF_BUILD_TYPE Release)
	endif(WIN32)
endif(NOT DEFINED CEF_BUILD_TYPE)

if(NOT TARGET cef)
    function(CEF_TARGET NAME)
        add_dependencies(${NAME} libcef_dll_wrapper)

        add_custom_command(
            TARGET ${NAME}
			POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CEF_PATH}/Resources" "$<TARGET_FILE_DIR:${NAME}>/chromium"
            COMMENT "Copying CEF resources..."
            VERBATIM
        )

		set(CEF_LIB_NAMES)
		set(LIB_ROOT_PATH "${CEF_PATH}/Release")
		
        file(GLOB CEF_LIBS "${LIB_ROOT_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}*${CMAKE_SHARED_LIBRARY_SUFFIX}")
		foreach(filepath ${CEF_LIBS})
			file(RELATIVE_PATH FILE_NAME "${LIB_ROOT_PATH}" "${filepath}")
			
			# Don't copy the pdf plugin, we don't need it
			if (NOT "${FILE_NAME}" STREQUAL "${CMAKE_SHARED_LIBRARY_PREFIX}pdf${CMAKE_SHARED_LIBRARY_SUFFIX}")
				set(CEF_LIB_NAMES ${CEF_LIB_NAMES} "${FILE_NAME}")
			endif (NOT "${FILE_NAME}" STREQUAL "${CMAKE_SHARED_LIBRARY_PREFIX}pdf${CMAKE_SHARED_LIBRARY_SUFFIX}")
		endforeach(filepath ${CEF_LIBS})
		
        list(LENGTH CEF_LIB_NAMES CEF_LIBS_SIZE)
        set(i 0)

        foreach(lib IN LISTS CEF_LIB_NAMES)
            math(EXPR i "${i} + 1")
            add_custom_command(
                TARGET ${NAME}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CEF_PATH}/${CEF_BUILD_TYPE}/${lib}" "$<TARGET_FILE_DIR:${NAME}>"
                COMMENT "[${i}/${CEF_LIBS_SIZE}] Copying CEF libraries..."
                VERBATIM
            )
        endforeach(lib IN LISTS CEF_LIBS)

        if(WIN32)
            target_link_libraries(${NAME} cef)
        elseif(UNIX)
            find_package(PkgConfig)

            if(PKG_CONFIG_FOUND)
                pkg_check_modules(GTK gtk+-2.0)
            endif(PKG_CONFIG_FOUND)

            target_include_directories(${NAME} PUBLIC ${GTK_INCLUDE_DIRS})
            target_link_libraries(${NAME} cef_dll_wrapper cef ${GTK_LIBRARIES})
        endif(WIN32)
    endfunction(CEF_TARGET)

    if(NOT EXISTS ${CEF_PATH}/include)
        message(FATAL_ERROR "The passed CEF_PATH is not valid!")
    endif(NOT EXISTS ${CEF_PATH}/include)

    add_library(cef IMPORTED SHARED GLOBAL)
    add_library(cef_dll_wrapper IMPORTED STATIC GLOBAL)
    add_dependencies(cef_dll_wrapper libcef_dll_wrapper)

    if(WIN32)
        set_target_properties(cef PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CEF_PATH}"
            IMPORTED_IMPLIB "${CEF_PATH}/Release/libcef.lib"
            IMPORTED_IMPLIB_DEBUG "${CEF_PATH}/Debug/libcef.lib"
            IMPORTED_LOCATION "${CEF_PATH}/Release/libcef.dll"
            IMPORTED_LOCATION_DEBUG "${CEF_PATH}/Debug/libcef.dll"
        )

        set_target_properties(cef_dll_wrapper PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CEF_PATH}"
            IMPORTED_IMPLIB "${CEF_PATH}/out/Release/lib/libcef_dll_wrapper.lib"
            IMPORTED_IMPLIB_DEBUG "${CEF_PATH}/out/Debug/lib/libcef_dll_wrapper.lib"
            IMPORTED_LINK_INTERFACE_LIBRARIES cef
        )
    elseif(UNIX)
        set_target_properties(cef PROPERTIES
            INTERFACE_LINK_LIBRARIES "-Wl,-rpath ."
            IMPORTED_LOCATION "${CEF_PATH}/${CEF_BUILD_TYPE}/libcef.so"
        )

        set_target_properties(cef_dll_wrapper PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CEF_PATH}"
            IMPORTED_LOCATION "${CEF_PATH}/out/${CEF_BUILD_TYPE}/obj.target/libcef_dll_wrapper.a"
        )
    endif(WIN32)

    if(NOT TARGET libcef_dll_wrapper)
        if(UNIX)
            add_custom_target(
                libcef_dll_wrapper
                COMMAND make libcef_dll_wrapper "BUILDTYPE=${CEF_BUILD_TYPE}"
                WORKING_DIRECTORY ${CEF_PATH}
                COMMENT "Building libcef_dll_wrapper.a ..."
            )
        elseif(WIN32)
            include_external_msproject(
                libcef_dll_wrapper
                "${CEF_PATH}/libcef_dll_wrapper.vcxproj"
            )
        endif(UNIX)

		set_target_properties(libcef_dll_wrapper
			PROPERTIES
				FOLDER "3rdparty"
		)
    endif(NOT TARGET libcef_dll_wrapper)
	
	# Define install files
	install(DIRECTORY "${CEF_PATH}/Resources/locales" DESTINATION ${BINARY_DESTINATION}/chromium)
	file(GLOB RESOURCE_FILES "${CEF_PATH}/Resources/*")
	foreach(resource ${RESOURCE_FILES})
		if (NOT IS_DIRECTORY "${resource}")
			install(FILES "${resource}" DESTINATION ${BINARY_DESTINATION}/chromium)
		endif (NOT IS_DIRECTORY "${resource}")
	endforeach(resource)

	foreach(config Release Debug)
		set(CEF_LIB_NAMES)
		set(LIB_ROOT_PATH "${CEF_PATH}/${config}")
		
		file(GLOB CEF_LIBS "${LIB_ROOT_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}*${CMAKE_SHARED_LIBRARY_SUFFIX}")
		foreach(filepath ${CEF_LIBS})
			file(RELATIVE_PATH FILE_NAME "${LIB_ROOT_PATH}" "${filepath}")
			
			# Don't copy the pdf plugin, we don't need it
			if (NOT "${FILE_NAME}" STREQUAL "${CMAKE_SHARED_LIBRARY_PREFIX}pdf${CMAKE_SHARED_LIBRARY_SUFFIX}")
				set(CEF_LIB_NAMES ${CEF_LIB_NAMES} "${FILE_NAME}")
			endif (NOT "${FILE_NAME}" STREQUAL "${CMAKE_SHARED_LIBRARY_PREFIX}pdf${CMAKE_SHARED_LIBRARY_SUFFIX}")
		endforeach(filepath ${CEF_LIBS})
		
		list(LENGTH CEF_LIB_NAMES CEF_LIBS_SIZE)
		set(i 0)

		foreach(lib IN LISTS CEF_LIB_NAMES)
			install(FILES "${LIB_ROOT_PATH}/${lib}" DESTINATION ${BINARY_DESTINATION} CONFIGURATIONS ${config})
		endforeach(lib IN LISTS CEF_LIBS)
	endforeach(config)
endif(NOT TARGET cef)