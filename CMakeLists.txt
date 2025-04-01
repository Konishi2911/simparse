cmake_minimum_required(VERSION 3.20)
project(SIMPARSE VERSION 0.1.0 LANGUAGES C CXX)

# Obtain the commit hash
execute_process(COMMAND git rev-parse HEAD
	OUTPUT_VARIABLE GIT_REV
	OUTPUT_STRIP_TRAILING_WHITESPACE
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
configure_file(${PROJECT_SOURCE_DIR}/include/version.hpp.in ${PROJECT_BINARY_DIR}/version.hpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_compile_options(-Wall -Wextra ${OpenMP_CXX_FLAGS})
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Google Testing Framework
find_package(GTest)
if (${GTest_FOUND}) 
	enable_testing()
	add_subdirectory(test)
endif()