


SET(DEF_OUT_FILES)
FOREACH(file ${file_root_def_files})
	FILE(RELATIVE_PATH FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR} ${file})
	
	SET(OUTPUT "${GENERATED_SOURCE_DIR}/code/${FILE_NAME}.h")

	# For some reason this is needed...
	GET_FILENAME_COMPONENT(DIRECTORY_PATH ${OUTPUT} PATH)
	FILE(MAKE_DIRECTORY ${DIRECTORY_PATH})

	get_filename_component(FILENAME ${file} NAME)

	STRING(REPLACE "-" "_" FILENAME ${FILENAME})
	STRING(REPLACE "." "_" FILENAME ${FILENAME})
	STRING(REPLACE "#" "_" FILENAME ${FILENAME})
	
	list(FIND file_root_def_files_binary ${file} file_index)
	
	SET(OPTIONS)
	if (file_index EQUAL -1)
		set(OPTIONS -text)
	else(file_index EQUAL -1)
		SET(OPTIONS)
	endif(file_index EQUAL -1)
	
	ADD_CUSTOM_COMMAND(
		OUTPUT ${OUTPUT}
		COMMAND embedfile ${OPTIONS} "${file}" "${OUTPUT}" "Default_${FILENAME}"
		DEPENDS ${file}
		COMMENT "Generating string file for ${file}"
		)
	
	LIST(APPEND DEF_OUT_FILES ${OUTPUT})
ENDFOREACH(file ${file_root_def_files})

SOURCE_GROUP("Generated Files\\Default Files" FILES ${DEF_OUT_FILES})
