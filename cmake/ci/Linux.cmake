if(NAME MATCHES "Integrated")
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

if(DAILY AND NOT INTEGRATED)
	step(${T_TARGET} gcovr)
	step(${T_TARGET} cloc.report)
endif()
