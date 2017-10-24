add_library(morphism
aut.cpp
	iso.cpp)

target_compile_features(morphism
	PUBLIC
		cxx_auto_type
		cxx_range_for)

	#target_link_libraries(morphism ${CMAKE_SOURCE_DIR}/lib/libbliss.a)
target_link_libraries(morphism blisslib)
add_library(dtrace::morphism ALIAS morphism)
