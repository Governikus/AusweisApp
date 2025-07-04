if("Vanilla" IN_LIST NAMES)
	message(STATUS "Use libraries: System")
	return()
elseif("Qt" IN_LIST NAMES)
	message(STATUS "Use libraries: Qt")
	find_program(AQT aqt REQUIRED)
else()
	message(STATUS "Use libraries: Governikus")
	if(DEFINED ENV{JENKINS_HOME})
		message(STATUS "Use libraries: Jenkins")
		return()
	endif()
endif()

if(EXISTS ${T_LIBS_DIR})
	step(${CMAKE_COMMAND} -E rm -r ${T_LIBS_DIR})
endif()

if(AQT)
	if(NOT QT)
		message(FATAL_ERROR "Provide version via -DQT=")
	endif()
	step(${AQT} install-qt -O ${T_LIBS_DIR} linux desktop ${QT} --modules qtwebsockets qtscxml qtconnectivity qtshadertools)
	step(${CMAKE_COMMAND} -E create_symlink ${QT}/gcc_64 ${T_LIBS_DIR}/dist)
	return()
endif()



if("FreeBSD" IN_LIST NAMES)
	set(PRESET ci-debug)
elseif("Android" IN_LIST NAMES)
	set(PRESET ci-android)
elseif("iOS" IN_LIST NAMES)
	if("Simulator" IN_LIST NAMES)
		set(PRESET ci-ios-simulator)
		if("arm64" IN_LIST NAMES)
			set(PRESET ${PRESET}-arm64)
		endif()
	else()
		set(PRESET ci-ios)
	endif()

	step(security unlock-keychain $ENV{KEYCHAIN_CREDENTIALS} $ENV{HOME}/Library/Keychains/login.keychain-db)
else()
	set(PRESET ci-release)
endif()

CALC_CHECKSUM(SALT ${CMAKE_CURRENT_LIST_FILE})
if(CMAKE_PARENT_LIST_FILE STREQUAL CMAKE_SCRIPT_MODE_FILE OR PROPAGATE)
	set(cfg ${T_CFG_LIBS})
else()
	set(cfg ${T_CFG_LIBS_DEPS})
endif()
step(${cfg} --preset ${PRESET} -DSALT=${SALT} NO_ECHO)
find_package(Governikus REQUIRED PATHS ${T_LIBS_DIR} NO_CMAKE_PATH NO_CMAKE_SYSTEM_PATH)
set(CACHED_TARBALL ${CACHE_DIR}/${LIBS_TARBALL})

block()
	include(Files)
endblock()

if(EXISTS "${CACHED_TARBALL}")
	message(STATUS "Use cached libraries: ${LIBS_TARBALL}")
else()
	download_pkg("${CACHED_TARBALL}")
endif()

if(NOT EXISTS "${CACHED_TARBALL}")
	message(STATUS "Build libraries...")
	step(${CMAKE_COMMAND} --build ${T_LIBS_DIR} --target compress)
	step(${CMAKE_COMMAND} -E copy_if_different "${T_LIBS_DIR}/${LIBS_TARBALL}" "${CACHED_TARBALL}")
	if(DEFINED ENV{GITLAB_CI})
		upload_pkg("${CACHED_TARBALL}")
	endif()
endif()

step(${CMAKE_COMMAND} -E rm -r ${T_LIBS_DIR})
step(${CMAKE_COMMAND} -E make_directory ${T_LIBS_DIR})
step(${CMAKE_COMMAND} -E tar xf "${CACHED_TARBALL}" CHDIR ${T_LIBS_DIR})
