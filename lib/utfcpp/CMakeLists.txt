
SET(HEADERS
	source/utf8.h
	source/utf8/checked.h
	source/utf8/core.h
	source/utf8/unchecked.h
)

add_library(utfcpp dummy.cpp ${HEADERS})

target_include_directories(utfcpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/source)

set_target_properties(utfcpp
	PROPERTIES
		FOLDER "3rdparty"
)

