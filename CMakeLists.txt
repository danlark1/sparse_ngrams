cmake_minimum_required(VERSION 3.7)
project(SPARSE_NGRAMS)

option(SPARSE_NGRAMS_TESTING "Building the tests." OFF)
option(SPARSE_NGRAMS_SANITIZE "Building the library with sanitizers." OFF)
option(SPARSE_NGRAMS_BUILD_LIBCXX "Building the library with libcxx." OFF)
option(SPARSE_NGRAMS_ENABLE_FUZZING "Building the library with fuzzing." OFF)

set(CMAKE_CXX_STANDARD 17)
add_library(
    sparse_ngrams
    ${PROJECT_SOURCE_DIR}/src/sparse_ngrams.cc
#   ${PROJECT_SOURCE_DIR}/src/substring_prefilter.cc
#   ${PROJECT_SOURCE_DIR}/src/regexp_prefilter.cc
)
target_include_directories(sparse_ngrams PUBLIC ${PROJECT_SOURCE_DIR}/include)

if (SPARSE_NGRAMS_TESTING)
    enable_testing()
    if (NOT CMAKE_BUILD_TYPE)
        message(STATUS "No build type selected, default to Release")
        set(CMAKE_BUILD_TYPE "Release")
    endif()
    if (SPARSE_NGRAMS_SANITIZE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fno-sanitize-recover=all")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Wpedantic -Werror -Wno-gnu-zero-variadic-macro-arguments")

    if (SPARSE_NGRAMS_BUILD_LIBCXX AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        message(STATUS "Using libcxx as a default standard C++ library")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()

    set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
    set(BENCHMARK_ENABLE_TESTING OFF)
    message(STATUS "gtest tests are off")

    add_subdirectory(benchmark)
    add_subdirectory(googletest)
    include_directories(testing)
    #include_directories(benches)

    #add_executable(benchmark_ngrams benches/benchmark_ngrams.cpp)
    #target_link_libraries(benchmark_ngrams benchmark::benchmark gtest sparse_ngrams)

    set(TEST_SOURCES testing/sparse_ngrams_test.cc)
    add_executable(test_ngrams ${TEST_SOURCES})
    target_link_libraries(test_ngrams gtest gmock gtest_main sparse_ngrams)
    add_test(NAME test_ngrams COMMAND test_ngrams)
endif()

if(SPARSE_NGRAMS_ENABLE_FUZZING)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
    set(BENCHMARK_ENABLE_TESTING OFF)
    message(STATUS "gtest tests are off")
    add_subdirectory(benchmark)
    add_subdirectory(googletest)
    include_directories(testing)
    add_subdirectory(fuzz)
endif()
