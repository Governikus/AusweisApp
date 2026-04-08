if(RELEASE)
	return()
endif()

block()
	include(Libraries)
endblock()

set(CHECK_SH ${CMAKE_SOURCE_DIR}/ci/check_formatting.sh)

step(${T_CFG} --preset ci-formatting)
if(REVIEW)
	IMPORT_PATCH(_unused --clean-dir-only)
	step(${CHECK_SH} ${PENDING_PATCH} CHDIR ${CMAKE_SOURCE_DIR})
endif()

# If script was modified we need to re-check with modified script again.
step(${CHECK_SH} CHDIR ${CMAKE_SOURCE_DIR} ENV NO_PATCH=ON)

step(${CMAKE_COMMAND} -DCMD=CHECK_FAILURE_CODES -P ${CMAKE_DIR}/cmd.cmake CHDIR ${CMAKE_SOURCE_DIR})
