include(FetchContent)

# reflect lib
FetchContent_Declare(
    reflection_lib
    URL https://github.com/qlibs/reflect/archive/refs/tags/v1.2.2.tar.gz
)
FetchContent_MakeAvailable(reflection_lib)

include_directories(
    ${reflection_lib_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

# use opencv lib already installed through docker, reduces compilation time
find_package(OpenCV REQUIRED)

# gtest lib
FetchContent_Declare(
    gtest_lib
    URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
)
FetchContent_MakeAvailable(gtest_lib)

add_executable(
    tests
    ${CMAKE_SOURCE_DIR}/test/test_pipeline.cpp
    ${CMAKE_SOURCE_DIR}/test/test_utils.cpp
    ${CMAKE_SOURCE_DIR}/src/lib/pipeline.cpp
    ${CMAKE_SOURCE_DIR}/src/lib/utils.cpp
)

target_include_directories(
    tests
    PUBLIC
    ${gtest_lib_SOURCE_DIR}/googletest/include
    ${gtest_lib_SOURCE_DIR}/googlemock/include
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    tests
    ${OpenCV_LIBS}
    gtest_main
    gtest
)

include(GoogleTest)
gtest_discover_tests(tests)

add_executable(
    gamma
    main.cpp
    ${CMAKE_SOURCE_DIR}/src/lib/pipeline.cpp
    ${CMAKE_SOURCE_DIR}/src/lib/utils.cpp
)

target_link_libraries(gamma ${OpenCV_LIBS})
target_include_directories(
    gamma
    PUBLIC
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)
