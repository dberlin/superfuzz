cmake_minimum_required(VERSION 2.8.12.20131211 FATAL_ERROR)
project(superfuzz CXX)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
find_package(CXXFeatures)

set(needed_features
	CXXFeatures_auto
	CXXFeatures_decltype)

foreach (i ${needed_features})
	if (NOT ${i}_FOUND)
		message(FATAL_ERROR "CXX features \"${i}\" is not supported by the compiler")
	endif()
endforeach()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_COMPILER_FLAGS}")

include_directories(include)
add_subdirectory(src)
