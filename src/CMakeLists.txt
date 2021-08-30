file( GLOB_RECURSE SRCS *.c *.cpp *.cc *.h *.hpp )
add_library( ${CMAKE_PROJECT_NAME} ${SRCS} )
# add_executable( ${CMAKE_PROJECT_NAME} main.cpp )
# target_link_libraries( ${CMAKE_PROJECT_NAME} ${EXTRA_LIBS} )
