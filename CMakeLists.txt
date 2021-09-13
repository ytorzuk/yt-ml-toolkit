project( yt_ml_toolkit )
cmake_minimum_required( VERSION 3.14 )

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(GSL
    GIT_REPOSITORY "https://github.com/microsoft/GSL"
    GIT_TAG "v3.1.0"
)
FetchContent_MakeAvailable(GSL)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)
add_subdirectory(tests)
