if(UNIX AND NOT MACOS AND NOT CYGWIN)
	if(CMAKE_VERSION VERSION_LESS "4.1.1")
		find_package(PkgConfig REQUIRED)
		pkg_check_modules(PCSC REQUIRED IMPORTED_TARGET libpcsclite)
	else()
		cmake_pkg_config(IMPORT libpcsclite REQUIRED NAME PCSC)
		get_target_property(PCSC_LIBRARIES PkgConfig::PCSC INTERFACE_LINK_LIBRARIES)
	endif()

else()
	add_library(PkgConfig::PCSC INTERFACE IMPORTED)

	if(MINGW OR CYGWIN)
		set(PCSC_LIBRARIES -lwinscard)

	elseif(MSVC OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
		set(PCSC_LIBRARIES winscard.lib)

	elseif(MACOS)
		find_library(PCSC_LIBRARIES PCSC)
		set_target_properties(PkgConfig::PCSC PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${PCSC_LIBRARIES}/Headers/")
	endif()

	set_target_properties(PkgConfig::PCSC PROPERTIES INTERFACE_LINK_LIBRARIES "${PCSC_LIBRARIES}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PCSC DEFAULT_MSG PCSC_LIBRARIES)
mark_as_advanced(PCSC_LIBRARIES)
