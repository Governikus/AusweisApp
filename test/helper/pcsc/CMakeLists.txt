if(LINUX OR BSD)
	file(GLOB SRC "*.cpp")

	add_library(Pcsc SHARED "${SRC}")
	target_link_libraries(Pcsc ${Qt}::Core PkgConfig::PCSC)
endif()
