cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
#cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
#cmake_policy(SET CMP0048 NEW)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(dtrace VERSION 0.1 LANGUAGES CXX)

include_directories("include")

#add_subdirectory("test")

#find_package(Boost COMPONENTS chrono system REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})
#link_directories(${Boost_LIBRARY_DIR})

add_subdirectory("lib/bliss")
add_subdirectory("src")

add_executable(dtrace src/main.cpp)
target_link_libraries(dtrace dtrace::graph)
#target_link_libraries(dtrace dtrace::embedding)
target_link_libraries(dtrace 
	dtrace::bruteforce
  	dtrace::lib)

add_executable(graph_meta src/graph_meta/main.cpp)
target_link_libraries(graph_meta dtrace::lib)

add_executable(autcheck src/aut_checker/main.cpp)
target_link_libraries(autcheck morphism)
target_compile_features(autcheck
	PUBLIC
		cxx_auto_type
		cxx_range_for)

add_executable(test_dtrace test/main.cpp test/embedding/traversor.cpp)
target_link_libraries(test_dtrace dtrace::bruteforce dtrace::graph
	#	${Boost_LIBRARIES}
  	dtrace::lib)
target_compile_features(test_dtrace
	PUBLIC
		cxx_auto_type
		cxx_range_for)
