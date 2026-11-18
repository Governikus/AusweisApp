block()
	include(Libraries)
endblock()

if("GNU" IN_LIST NAMES)
	set(GNU ON)
elseif(("clang" IN_LIST NAMES OR "clang-cl" IN_LIST NAMES) AND DEFINED ENV{LLVM})
	set(PATH "$ENV{LLVM}")
endif()

if("dev" IN_LIST NAMES)
	set(PRESET ci-win-debug)
else()
	set(PRESET ci-win-release)
endif()

if("arm64" IN_LIST NAMES)
	set(ARM64 ON)
endif()

if(NOT GNU)
	set(VCVARS_PACKAGE cmd /c vcvarsall.bat amd64 && call)
	if(ARM64)
		set(VCVARS_BUILD cmd /c vcvarsall.bat amd64_arm64 && call)
		set(PRESET ${PRESET}-arm64)
	else()
		set(VCVARS_BUILD ${VCVARS_PACKAGE})
	endif()
endif()


step(${VCVARS_BUILD} ${T_CFG} --preset ${PRESET} PATH "${PATH}")
step(${VCVARS_BUILD} ${T_BUILD})
step(${VCVARS_PACKAGE} ${T_TARGET} package)
step(${VCVARS_PACKAGE} ${T_TARGET} package.sign)

file(GLOB FILES "${T_BUILD_DIR}/*.msi")
hashsum(${FILES})
step(${CMAKE_COMMAND} -DARM64=${ARM64} -DCMD=GENERATE_APPCAST -P ${CMAKE_DIR}/cmd.cmake CHDIR ${T_BUILD_DIR})

if(NOT RELEASE)
	step(${CMAKE_COMMAND} -DCMD=CHECK_WIX_MSI -DFILE=${T_BUILD_DIR}/_CPack_Packages/win64/WIX/wix.log -P ${CMAKE_DIR}/cmd.cmake)

	set(VALIDATION_FILE ${T_BUILD_DIR}/_CPack_Packages/win64/WIX/validation.log)
	step(wix msi validate ${FILES} OUTPUT_FILE ${VALIDATION_FILE} RESULT _ignore)
	step(${CMAKE_COMMAND} -DCMD=CHECK_WIX_VALIDATION -DFILE=${VALIDATION_FILE} -P ${CMAKE_DIR}/cmd.cmake)

	if(NOT ARM64 OR $ENV{PROCESSOR_ARCHITECTURE} STREQUAL "ARM64")
		step(${T_CTEST} ENV QT_PLUGIN_PATH=${WORKSPACE}/libs/dist/plugins QML2_IMPORT_PATH=${WORKSPACE}/libs/dist/qml PATH "${WORKSPACE}/libs/dist/bin")
	endif()
endif()
