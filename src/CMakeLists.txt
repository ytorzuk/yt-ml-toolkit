file( GLOB_RECURSE SRCS *.c *.cpp *.cc *.h *.hpp )
add_library( ${CMAKE_PROJECT_NAME} ${SRCS} )
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GSL)
