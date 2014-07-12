if(NOT DEFINED CEF_BUILD_TYPE)
    if(UNIX)
        set(CEF_BUILD_TYPE Release)
    else(UNIX)
        set(CEF_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
    endif(UNIX)
endif(NOT DEFINED CEF_BUILD_TYPE)

function(CEF_TARGET NAME)
    if(CEF_PATH)
        if(NOT EXISTS ${CEF_PATH}/include)
            message(FATAL_ERROR "The passed CEF_PATH is not valid!")
        endif(NOT EXISTS ${CEF_PATH}/include)
        
        target_include_directories(${NAME} PUBLIC ${CEF_PATH})
    endif(CEF_PATH)

    if(UNIX)
        find_package(PkgConfig)

        if(PKG_CONFIG_FOUND)
            pkg_check_modules(GTK gtk+-2.0)
        endif(PKG_CONFIG_FOUND)

        target_include_directories(${NAME} PUBLIC ${GTK_INCLUDE_DIRS})
        target_link_libraries(${NAME} ${GTK_LIBRARIES})
        
        if(CEF_PATH)
            set(CEF_DLL_WRAPPER ${CEF_PATH}/out/${CEF_BUILD_TYPE}/obj.target/libcef_dll_wrapper.a)
            
            add_dependencies(${NAME} libcef_dll_wrapper)
            target_link_libraries(${NAME} "-Wl,-rpath ." ${CEF_PATH}/${CEF_BUILD_TYPE}/libcef.so ${CEF_DLL_WRAPPER})
        endif(CEF_PATH)
    elseif(WIN32 AND CEF_PATH)
        target_link_libraries(${NAME} ${CEF_PATH}/${CEF_BUILD_TYPE}/libcef.lib ${CEF_PATH}/out/${CEF_BUILD_TYPE}/lib/libcef_dll_wrapper.lib)
    endif(UNIX)
endfunction(CEF_TARGET)

if(CEF_PATH AND UNIX AND NOT TARGET libcef_dll_wrapper)
    add_custom_target(
        libcef_dll_wrapper
        COMMAND make libcef_dll_wrapper "BUILDTYPE=${CEF_BUILD_TYPE}"
        WORKING_DIRECTORY ${CEF_PATH}
        COMMENT "Building libcef_dll_wrapper.a ..."
    )
endif(CEF_PATH AND UNIX AND NOT TARGET libcef_dll_wrapper)