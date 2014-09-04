
# Find the lua executable
find_program(LUA_EXECUTABLE lua)

set(GLLOADGEN_PATH "" CACHE PATH "The directory where the glloadgen files are found")
mark_as_advanced(GLLOADGEN_PATH)

set(GLLOADGEN_DEFAULT ${CMAKE_CURRENT_SOURCE_DIR}/graphics/gl/glloadgen)

set(LOADGEN_FILES
    gl_2_1.cpp
    gl_2_1.hpp
    gl_3_2.cpp
    gl_3_2.hpp
)

if(LUA_EXECUTABLE AND IS_DIRECTORY ${GLLOADGEN_PATH})
    # Create the output directory
    file(MAKE_DIRECTORY "${GENERATED_SOURCE_DIR}/code/glloadgen")

    SET(GLLOADGEN_FILES)

    foreach(file ${LOADGEN_FILES})
        set(GLLOADGEN_FILES ${GLLOADGEN_FILES} "${GENERATED_SOURCE_DIR}/code/glloadgen/${file}")
    endforeach(file)

    add_custom_command(OUTPUT ${GLLOADGEN_FILES}
        COMMAND "${LUA_EXECUTABLE}" "${GLLOADGEN_PATH}/LoadGen.lua" -spec=gl
        -prefix=gl2 -version=2.1 -style=func_cpp
        -extfile="${GLLOADGEN_DEFAULT}/glExtensions.txt" 2_1

        COMMAND "${LUA_EXECUTABLE}" "${GLLOADGEN_PATH}/LoadGen.lua" -spec=gl
        -prefix=gl3 -version=3.2 -style=func_cpp -profile=core 3_2

        DEPENDS "${GLLOADGEN_DEFAULT}/glExtensions.txt"
        WORKING_DIRECTORY "${GENERATED_SOURCE_DIR}/code/glloadgen/"
        COMMENT "Generating OpenGL function loaders...")
else(LUA_EXECUTABLE AND IS_DIRECTORY ${GLLOADGEN_PATH})
    SET(GLLOADGEN_FILES)

    foreach(file ${LOADGEN_FILES})
        set(GLLOADGEN_FILES ${GLLOADGEN_FILES} "${GLLOADGEN_DEFAULT}/${file}")
    endforeach(file)
endif(LUA_EXECUTABLE AND IS_DIRECTORY ${GLLOADGEN_PATH})

SOURCE_GROUP("Generated Files\\glloadgen Files" FILES ${GLLOADGEN_FILES})
