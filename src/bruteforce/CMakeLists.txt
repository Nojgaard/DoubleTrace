add_library(bruteforce
	embedding.cpp 
	face_trace.cpp)

target_compile_features(bruteforce
	PUBLIC
		cxx_auto_type
		cxx_range_for
		cxx_noexcept)

add_library(dtrace::bruteforce ALIAS bruteforce)
