
find_package(Boost REQUIRED)

include(util)

ADD_IMPORTED_LIB(boost "${Boost_INCLUDE_DIRS}" "${Boost_LIBRARIES}" STATIC)

SET(BOOST_LIBS boost CACHE INTERNAL "")
