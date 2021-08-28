project( yt_ml_toolkit )
cmake_minimum_required( VERSION 3.14 )

set(CMAKE_CXX_STANDARD 11)

include_directories( ${CMAKE_SOURCE_DIR}/includes )
file( GLOB_RECURSE SRCS *.c *.cpp *.cc *.h *.hpp )

add_library( ${CMAKE_PROJECT_NAME} ${SRCS} )

#add_executable( ${CMAKE_PROJECT_NAME} main.cpp )
#target_link_libraries( ${CMAKE_PROJECT_NAME}  ${EXTRA_LIBS} )
