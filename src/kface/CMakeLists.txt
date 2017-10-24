add_library(kfacelib 
	Find.cpp 
	LocalRotation.cpp
  	Trace.cpp
  	prune/OOrderlyGen.cpp
  	AutGroup.cpp
	prune/None.cpp
  	RotationSystem.cpp
  	prune/KNone.cpp
  	prune/Gap.cpp
  	prune/All.cpp
	Gap.cpp
	GapHist.cpp
	)

target_link_libraries(kfacelib dtrace::lib morphism)
target_compile_features(kfacelib PUBLIC cxx_auto_type cxx_range_for)

add_library(kface::lib ALIAS kfacelib)

add_executable(kface main.cpp)
target_link_libraries(kface kface::lib)

add_subdirectory("test")
