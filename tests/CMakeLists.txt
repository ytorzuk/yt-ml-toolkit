file(GLOB_RECURSE test_SRCS *.c *.cpp *.cc *.h *.hpp)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  hello_test
  ${test_SRCS}
)
target_link_libraries(
  hello_test
  gtest_main
  ${CMAKE_PROJECT_NAME}
)

include(GoogleTest)
gtest_discover_tests(hello_test)
