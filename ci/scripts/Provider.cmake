if(PROVIDER MATCHES "(bld|build)")
	set(PROVIDER_NO_DOWNLOAD ON)
elseif(PROVIDER MATCHES "(dl|download)")
	set(PROVIDER_NO_BUILD ON)
endif()

include(Libraries)

if(EXISTS "${T_LIBS_DIR}")
	step(${CMAKE_COMMAND} -E rename "${T_LIBS_DIR}/dist" "${CMAKE_BINARY_DIR}/provider")
endif()
