file(GLOB CORE_SOURCES "*.cpp" "*.hpp")
add_library(core SHARED ${CORE_SOURCES})

find_package( Boost REQUIRED COMPONENTS iostreams)
target_link_libraries( core ${Boost_LIBRARIES})
