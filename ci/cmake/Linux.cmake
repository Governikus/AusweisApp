block(PROPAGATE GOVERNIKUS_QT)
	include(Libraries)
endblock()

if("Integrated" IN_LIST NAMES)
	set(INTEGRATED ON)
endif()

if(INTEGRATED)
	set(PRESET ci-integrated)
else()
	set(PRESET ci-linux)
endif()

step(${T_CFG} --preset ${PRESET})
step(${T_BUILD})
step(${T_CTEST} ENV QT_PLUGIN_PATH=${WORKSPACE}/libs/dist/plugins QML2_IMPORT_PATH=${WORKSPACE}/libs/dist/qml)
step(${CMAKE_COMMAND} --install ${T_BUILD_DIR} ENV DESTDIR=${WORKSPACE}/install)

if(GOVERNIKUS_QT)
	step(${T_TARGET} gcovr)
endif()

if(DAILY)
	if(DEFINED ENV{JENKINS_HOME})
		step(${T_TARGET} cloc.report)
	endif()
endif()
