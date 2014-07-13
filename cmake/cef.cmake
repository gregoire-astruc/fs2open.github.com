if(NOT TARGET cef)
    set(_CEF_LOADED ON)

    function(CEF_TARGET NAME)
        target_link_libraries(${NAME} cef_dll_wrapper cef)
        #add_dependencies(${NAME} cef_dll_wrapper)
        #target_include_directories(${NAME} PUBLIC "$<TARGET_PROPERTY:cef_dll_wrapper,INTERFACE_INCLUDE_DIRECTORIES>")

        if(UNIX)
            find_package(PkgConfig)

            if(PKG_CONFIG_FOUND)
                pkg_check_modules(GTK gtk+-2.0)
            endif(PKG_CONFIG_FOUND)

            target_include_directories(${NAME} PUBLIC ${GTK_INCLUDE_DIRS})
            target_link_libraries(${NAME} ${GTK_LIBRARIES})
        endif(UNIX)
    endfunction(CEF_TARGET)

    if(NOT EXISTS ${CEF_PATH}/include)
        message(FATAL_ERROR "The passed CEF_PATH is not valid!")
    endif(NOT EXISTS ${CEF_PATH}/include)

    if(NOT DEFINED CEF_BUILD_TYPE)
        if(UNIX)
            # Default to Release on Unix
            set(CEF_BUILD_TYPE Release)
        else(UNIX)
            # At least on Windows these have to match...
            set(CEF_BUILD_TYPE "$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>")
        endif(UNIX)
    endif(NOT DEFINED CEF_BUILD_TYPE)


    add_library(cef IMPORTED SHARED GLOBAL)
    add_library(cef_dll_wrapper IMPORTED STATIC GLOBAL)
    add_dependencies(cef_dll_wrapper libcef_dll_wrapper)

    if(WIN32)
        set_target_properties(cef PROPERTIES
            IMPORTED_IMPLIB "${CEF_PATH}/${CEF_BUILD_TYPE}/libcef.lib"
            IMPORTED_LOCATION "${CEF_PATH}/${CEF_BUILD_TYPE}/libcef.dll"
        )

        set_target_properties(cef_dll_wrapper PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CEF_PATH}"
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
                "${CEF_PATH}/libcef_dll_wrapper.vcproj"
            )
        endif(UNIX)
    endif(NOT TARGET libcef_dll_wrapper)

    add_custom_command(
        TARGET libcef_dll_wrapper
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CEF_PATH}/Resources" "${EXECUTABLE_OUTPUT_PATH}/chromium"
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
            TARGET libcef_dll_wrapper
            COMMAND ${CMAKE_COMMAND} -E copy "${LIB}" "${EXECUTABLE_OUTPUT_PATH}"
            COMMENT "[${num}/${CEF_LIBS_SIZE}] Copying CEF libraries..."
            VERBATIM
        )
    endforeach(i)
endif(NOT TARGET cef)