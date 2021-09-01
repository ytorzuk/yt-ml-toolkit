project( yt_ml_toolkit )
cmake_minimum_required( VERSION 3.14 )

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)
add_subdirectory(tests)
