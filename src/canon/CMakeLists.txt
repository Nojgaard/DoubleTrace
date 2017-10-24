add_library(canon
	trace.cpp
	embedding.cpp
	emb_naive.cpp
	emb_brute.cpp
	)

target_link_libraries(canon dtrace::morphism)
target_compile_features(canon PUBLIC cxx_auto_type cxx_range_for)
add_library(dtrace::canon ALIAS canon)
