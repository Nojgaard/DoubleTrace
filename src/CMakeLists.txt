add_subdirectory("morphism")
add_subdirectory("kface")
#add_subdirectory("embedding")
#add_subdirectory("trace")
add_subdirectory("ctest")
add_subdirectory("benchmark")
add_subdirectory("canon")
add_subdirectory("bruteforce")
add_subdirectory("collect_gaps")
add_subdirectory("gap_hist")

add_library(graph graph.cpp embedding.cpp)

target_compile_features(graph
	PUBLIC
		cxx_auto_type
		cxx_range_for
		cxx_noexcept)

	#target_link_libraries(graph dtrace::trace)
add_library(dtrace::graph ALIAS graph)

add_library(lib
	local_trace.cpp
	traversor.cpp
	util.cpp
	io.cpp
	perm_list.cpp
	stat_printer.cpp)
target_compile_features(lib PUBLIC cxx_auto_type cxx_range_for)
target_link_libraries(lib 
	dtrace::canon
  	morphism
	)
add_library(dtrace::lib ALIAS lib)


