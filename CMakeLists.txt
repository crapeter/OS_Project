cmake_minimum_required(VERSION 3.11)
project(OS_Proj)
set(CMAKE_CXX_STANDARD 20)

# Dependencies
include(FetchContent)

# Find TBB - try both modern and legacy configurations
find_package(TBB REQUIRED COMPONENTS tbb)
if(NOT TBB_FOUND)
    # Fallback for older TBB versions
    find_package(TBB)
endif()

FetchContent_Declare(
    scheduling
    GIT_REPOSITORY https://github.com/dpuyda/scheduling
    GIT_TAG main
)

FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.0
)

set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON)
FetchContent_MakeAvailable(scheduling benchmark)

set(SCHEDULING_FUNCTIONS src/scheduling_functions.cpp)

# Main executable (doesn't need TBB)
add_executable(os_proj_main 
    src/main.cpp 
    ${SCHEDULING_FUNCTIONS}
)
target_include_directories(os_proj_main PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${scheduling_SOURCE_DIR}/include
)
target_link_libraries(os_proj_main PRIVATE 
    scheduling
)

# Benchmark executable
add_executable(os_proj_benchmarks 
    src/scheduling_benchmarks.cpp 
    src/tbb_scheduling.cpp 
    ${SCHEDULING_FUNCTIONS}
)
target_include_directories(os_proj_benchmarks PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${scheduling_SOURCE_DIR}/include
    ${benchmark_SOURCE_DIR}/include
)

# Modern TBB linking with fallback
if(TARGET TBB::tbb)
    message(STATUS "Linking with TBB::tbb")
    target_link_libraries(os_proj_benchmarks PRIVATE TBB::tbb)
elseif(TARGET tbb)
    message(STATUS "Linking with tbb (legacy)")
    target_link_libraries(os_proj_benchmarks PRIVATE tbb)
else()
    message(FATAL_ERROR "TBB library not found - tried TBB::tbb and tbb")
endif()

target_link_libraries(os_proj_benchmarks PRIVATE
    scheduling
    benchmark::benchmark
)