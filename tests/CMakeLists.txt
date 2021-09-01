file(GLOB_RECURSE test_SRCS *.c *.cpp *.cc *.h *.hpp)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
set(ADDITIONAL_LINKER_OPTS "stdc++fs")
endif()

enable_testing()

add_executable(
  ${CMAKE_PROJECT_NAME}_test
  ${test_SRCS}
)
target_link_libraries(
  ${CMAKE_PROJECT_NAME}_test
  gtest_main
  ${CMAKE_PROJECT_NAME}
  ${ADDITIONAL_LINKER_OPTS}
)
include(GoogleTest)
gtest_discover_tests(${CMAKE_PROJECT_NAME}_test)
