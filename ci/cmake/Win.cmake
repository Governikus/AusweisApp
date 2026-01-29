block()
	include(Libraries)
endblock()

if("GNU" IN_LIST NAMES)
	set(GNU ON)
elseif(("clang" IN_LIST NAMES OR "clang-cl" IN_LIST NAMES) AND DEFINED ENV{LLVM})
	set(PATH "$ENV{LLVM}")
endif()

if(NOT GNU)
	set(VCVARS cmd /c vcvarsall.bat amd64 && call)
endif()

if("dev" IN_LIST NAMES)
	set(PRESET ci-win-debug)
else()
	set(PRESET ci-win-release)
endif()

step(${VCVARS} ${T_CFG} --preset ${PRESET} PATH "${PATH}")
step(${VCVARS} ${T_BUILD})
step(${VCVARS} ${T_TARGET} package)
step(${VCVARS} ${T_TARGET} package.sign)

file(GLOB FILES "${T_BUILD_DIR}/*.msi")
hashsum(${FILES})
step(${CMAKE_COMMAND} -DCMD=GENERATE_APPCAST -P ${CMAKE_DIR}/cmd.cmake CHDIR ${T_BUILD_DIR})

if(NOT RELEASE)
	step(${CMAKE_COMMAND} -DCMD=CHECK_WIX_MSI -DFILE=${T_BUILD_DIR}/_CPack_Packages/win64/WIX/wix.log -P ${CMAKE_DIR}/cmd.cmake)

	set(VALIDATION_FILE ${T_BUILD_DIR}/_CPack_Packages/win64/WIX/validation.log)
	step(wix msi validate ${FILES} OUTPUT_FILE ${VALIDATION_FILE} RESULT _ignore)
	step(${CMAKE_COMMAND} -DCMD=CHECK_WIX_VALIDATION -DFILE=${VALIDATION_FILE} -P ${CMAKE_DIR}/cmd.cmake)

	step(${T_CTEST} ENV QT_PLUGIN_PATH=${WORKSPACE}/libs/dist/plugins QML2_IMPORT_PATH=${WORKSPACE}/libs/dist/qml PATH "${WORKSPACE}/libs/dist/bin")
endif()
